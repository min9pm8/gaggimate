import { useCallback, useContext, useEffect, useRef, useState } from 'preact/hooks';
import Card from '../../components/Card.jsx';
import { Spinner } from '../../components/Spinner.jsx';
import { ApiServiceContext } from '../../services/ApiService.js';
import { downloadJson } from '../../utils/download.js';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faCheck } from '@fortawesome/free-solid-svg-icons/faCheck';

function LocalUploadCard() {
  const [file, setFile] = useState(null);
  const [uploading, setUploading] = useState(false);
  const [uploadProgress, setUploadProgress] = useState(0);
  const [result, setResult] = useState(null);

  const handleUpload = useCallback(async () => {
    if (!file) return;
    setUploading(true);
    setResult(null);
    setUploadProgress(0);

    const formData = new FormData();
    formData.append('file', file, file.name);

    try {
      await new Promise((resolve, reject) => {
        const xhr = new XMLHttpRequest();
        let done = false;
        const finish = (msg) => {
          if (done) return;
          done = true;
          setResult({ success: true, message: msg || 'Upload complete — device is restarting' });
          resolve();
        };
        // After upload bytes are fully sent, the device writes to flash then reboots.
        // If no response arrives within 15s, assume success.
        xhr.upload.onload = () => {
          setTimeout(() => finish('Upload complete — device is restarting'), 15000);
        };
        xhr.open('POST', '/api/ota/upload');
        xhr.upload.onprogress = e => {
          if (e.lengthComputable) setUploadProgress(Math.round((e.loaded / e.total) * 100));
        };
        xhr.onload = () => {
          try {
            const data = JSON.parse(xhr.responseText);
            if (data.success) {
              finish('Upload complete — device is restarting');
            } else {
              if (!done) {
                done = true;
                setResult({ success: false, message: data.error || 'Upload failed' });
                resolve();
              }
            }
          } catch {
            finish('Upload complete — device is restarting');
          }
        };
        xhr.onerror = () => {
          finish('Upload complete — device is restarting');
        };
        xhr.send(formData);
      });
    } finally {
      setUploading(false);
      setUploadProgress(0);
    }
  }, [file]);

  const isFirmware = file && !file.name.endsWith('spiffs.bin');
  const isSpiffs = file && file.name.endsWith('spiffs.bin');

  return (
    <Card sm={12} title='Local Firmware Upload'>
      <p className='text-sm opacity-70'>
        Upload a locally-built firmware or filesystem image directly to the device. Build with{' '}
        <code>pio run -e display</code> and find the binary at{' '}
        <code>.pio/build/display/firmware.bin</code> or <code>spiffs.bin</code>.
      </p>
      <div className='form-control'>
        <label htmlFor='localFirmwareFile' className='mb-2 block text-sm font-medium'>
          Select <code>firmware.bin</code> or <code>spiffs.bin</code>
        </label>
        <input
          id='localFirmwareFile'
          type='file'
          accept='.bin'
          className='file-input file-input-bordered w-full'
          onChange={e => {
            setFile(e.target.files[0] || null);
            setResult(null);
          }}
          disabled={uploading}
        />
        {file && (
          <p className='mt-1 text-xs opacity-60'>
            {file.name} ({(file.size / 1024).toFixed(1)} KB) —{' '}
            {isFirmware ? 'Display firmware' : isSpiffs ? 'Filesystem (SPIFFS)' : 'Unknown type'}
          </p>
        )}
      </div>

      {uploading && (
        <div className='space-y-1'>
          <div className='bg-base-300 h-2 w-full overflow-hidden rounded'>
            <div
              className='bg-primary h-full transition-all duration-200'
              style={{ width: `${uploadProgress}%` }}
            />
          </div>
          <p className='text-xs opacity-60'>{uploadProgress}% uploaded</p>
        </div>
      )}

      {result && (
        <div className={`alert ${result.success ? 'alert-success' : 'alert-error'}`}>
          <span>{result.message}</span>
        </div>
      )}

      <div className='alert alert-warning'>
        <span>The device will restart automatically after a successful upload.</span>
      </div>

      <button
        type='button'
        className='btn btn-secondary'
        disabled={!file || uploading}
        onClick={handleUpload}
      >
        {uploading ? (
          <>
            <Spinner size={4} />
            Uploading...
          </>
        ) : (
          'Upload to Device'
        )}
      </button>
    </Card>
  );
}

const imageUrlToBase64 = async blob => {
  return new Promise((onSuccess, onError) => {
    try {
      const reader = new FileReader();
      reader.onload = function () {
        onSuccess(this.result);
      };
      reader.readAsDataURL(blob);
    } catch (e) {
      onError(e);
    }
  });
};

export function OTA() {
  const apiService = useContext(ApiServiceContext);
  const [isLoading, setIsLoading] = useState(true);
  const [submitting, setSubmitting] = useState(false);
  const [formData, setFormData] = useState({});
  const [phase, setPhase] = useState(0);
  const [progress, setProgress] = useState(0);

  const downloadSupportData = useCallback(async () => {
    const settingsResponse = await fetch(`/api/settings`);
    const data = await settingsResponse.json();
    delete data.wifiPassword;
    delete data.haPassword;
    const coredumpBlob = await fetch(`/api/core-dump`).then(r => r.blob());
    let coredump = await imageUrlToBase64(coredumpBlob);
    coredump = coredump.substring(coredump.indexOf('base64,') + 7);
    const supportFile = {
      settings: data,
      versions: formData,
      coredump,
    };
    const ts = Date.now();
    downloadJson(supportFile, `support-${ts}.dat`);
  }, [formData]);
  useEffect(() => {
    const listenerId = apiService.on('res:ota-settings', msg => {
      setFormData(msg);
      setIsLoading(false);
      setSubmitting(false);
    });
    return () => {
      apiService.off('res:ota-settings', listenerId);
    };
  }, [apiService]);
  useEffect(() => {
    const listenerId = apiService.on('evt:ota-progress', msg => {
      setProgress(msg.progress);
      setPhase(msg.phase);
    });
    return () => {
      apiService.off('evt:ota-progress', listenerId);
    };
  }, [apiService]);
  
  useEffect(() => {
    const listenerId = apiService.on('evt:history-rebuild-progress', msg => {
      setRebuildProgress({
        total: msg.total || 0,
        current: msg.current || 0,
        status: msg.status || ''
      });
      
      if (msg.status === 'completed' || msg.status === 'error') {
        setRebuilding(false);
        setRebuilt(msg.status === 'completed');
      }
    });
    return () => {
      apiService.off('evt:history-rebuild-progress', listenerId);
    };
  }, [apiService]);
  useEffect(() => {
    setTimeout(() => {
      apiService.send({ tp: 'req:ota-settings' });
    }, 500);
  }, [apiService]);

  const formRef = useRef();

  const onSubmit = useCallback(
    async e => {
      e.preventDefault();
      setSubmitting(true);
      const form = formRef.current;
      const formData = new FormData(form);
      apiService.send({ tp: 'req:ota-settings', update: true, channel: formData.get('channel') });
      setSubmitting(true);
    },
    [setFormData, formRef],
  );

  const onUpdate = useCallback(
    component => {
      apiService.send({ tp: 'req:ota-start', cp: component });
    },
    [apiService],
  );

  const [rebuilding, setRebuilding] = useState(false);
  const [rebuilt, setRebuilt] = useState(false);
  const [rebuildProgress, setRebuildProgress] = useState({ total: 0, current: 0, status: '' });
  const onHistoryRebuild = useCallback(async () => {
    setRebuilt(false);
    setRebuilding(true);
    setRebuildProgress({ total: 0, current: 0, status: 'starting' });
    apiService.send({ tp: 'req:history:rebuild' });
  }, [apiService]);

  if (isLoading) {
    return (
      <div className='flex w-full flex-row items-center justify-center py-16'>
        <Spinner size={8} />
      </div>
    );
  }

  if (phase > 0) {
    return (
      <div className='flex flex-col items-center gap-4 p-16'>
        <Spinner size={8} />
        <span className='text-xl font-medium'>
          {phase === 1
            ? 'Updating Display firmware'
            : phase === 2
              ? 'Updating Display filesystem'
              : phase === 3
                ? 'Updating controller firmware'
                : 'Finished'}
        </span>
        <span className='text-lg font-medium'>{phase === 4 ? 100 : progress}%</span>
        {phase === 4 && (
          <a href='/' className='btn btn-primary'>
            Back
          </a>
        )}
      </div>
    );
  }

  return (
    <>
      <div className='mb-4 flex flex-row items-center gap-2'>
        <h2 className='flex-grow text-2xl font-bold sm:text-3xl'>System & Updates</h2>
      </div>

      <form key='ota' method='post' action='/api/ota' ref={formRef} onSubmit={onSubmit}>
        <div className='grid grid-cols-1 gap-4 lg:grid-cols-12'>
          <LocalUploadCard />
          <Card sm={12} title='System Information'>
            <div className='flex flex-col space-y-4'>
              <label htmlFor='channel' className='text-sm font-medium'>
                Update Channel
              </label>
              <select id='channel' name='channel' className='select select-bordered w-full'>
                <option value='latest' selected={formData.channel === 'latest'}>
                  Stable
                </option>
                <option value='nightly' selected={formData.channel === 'nightly'}>
                  Nightly
                </option>
              </select>
            </div>

            <div className='flex flex-col space-y-4'>
              <label className='text-sm font-medium'>Hardware</label>
              <div className='input input-bordered bg-base-200 cursor-default break-words whitespace-normal'>
                {formData.hardware}
              </div>
            </div>

            <div className='flex flex-col space-y-4'>
              <label className='text-sm font-medium'>Controller Version</label>
              <div className='input input-bordered bg-base-200 cursor-default break-words whitespace-normal'>
                <span className='break-all'>{formData.controllerVersion}</span>
                {formData.controllerUpdateAvailable && (
                  <span className='text-primary font-bold break-all'>
                    (Update available: {formData.latestVersion})
                  </span>
                )}
              </div>
            </div>

            <div className='flex flex-col space-y-4'>
              <label className='text-sm font-medium'>Display Version</label>
              <div className='input input-bordered bg-base-200 cursor-default break-words whitespace-normal'>
                <span className='break-all'>{formData.displayVersion}</span>
                {formData.displayUpdateAvailable && (
                  <span className='text-primary font-bold break-all'>
                    (Update available: {formData.latestVersion})
                  </span>
                )}
              </div>
            </div>

            <div className='flex flex-col space-y-4'>
              <label className='text-sm font-medium'>Web UI Build</label>
              <div className='input input-bordered bg-base-200 cursor-default break-words whitespace-normal'>
                <span className='break-all'>{__SPIFFS_BUILD_TIME__}</span>
              </div>
            </div>

            {formData.spiffsTotal !== undefined && (
              <div className='flex flex-col space-y-2'>
                <label className='text-sm font-medium'>Storage (SPIFFS)</label>
                <div className='flex flex-col gap-1'>
                  <div className='bg-base-300 h-3 w-full overflow-hidden rounded'>
                    <div
                      className='bg-primary h-full transition-all'
                      style={{ width: `${formData.spiffsUsedPct || 0}%` }}
                    />
                  </div>
                  <div className='text-xs opacity-75'>
                    {((formData.spiffsUsed || 0) / 1024).toFixed(1)} KB /{' '}
                    {(formData.spiffsTotal / 1024).toFixed(1)} KB ({formData.spiffsUsedPct}%)
                  </div>
                </div>
              </div>
            )}

            {formData.sdTotal !== undefined && (
              <div className='flex flex-col space-y-2'>
                <label className='text-sm font-medium'>Storage (SD-Card)</label>
                <div className='flex flex-col gap-1'>
                  <div className='bg-base-300 h-3 w-full overflow-hidden rounded'>
                    <div
                      className='bg-primary h-full transition-all'
                      style={{ width: `${formData.sdUsedPct || 0}%` }}
                    />
                  </div>
                  <div className='text-xs opacity-75'>
                    {((formData.sdUsed || 0) / 1024 / 1024).toFixed(1)} MB /{' '}
                    {(formData.sdTotal / 1024 / 1024).toFixed(1)} MB ({formData.sdUsedPct}%)
                  </div>
                </div>
              </div>
            )}

            <div className='alert alert-warning'>
              <span>
                Make sure to backup your profiles from the profile screen before updating the
                display.
              </span>
            </div>
          </Card>
        </div>

        <div className='pt-4 lg:col-span-12'>
          <div className='flex flex-col flex-wrap gap-2 sm:flex-row'>
            <button type='submit' className='btn btn-primary' disabled={submitting}>
              Save & Refresh
            </button>
            <button
              type='submit'
              name='update'
              className='btn btn-secondary'
              disabled={!formData.displayUpdateAvailable || submitting}
              onClick={() => onUpdate('display')}
            >
              Update Display
            </button>
            <button
              type='submit'
              name='update'
              className='btn btn-secondary'
              disabled={!formData.controllerUpdateAvailable || submitting}
              onClick={() => onUpdate('controller')}
            >
              Update Controller
            </button>
            <button type='button' className='btn btn-outline' onClick={downloadSupportData}>
              Download Support Data
            </button>
            <button
              type='button'
              className='btn btn-outline'
              onClick={onHistoryRebuild}
              disabled={rebuilding}
            >
              Rebuild Shot History
              {rebuilding && (
                <>
                  <Spinner size={4} className='ml-2' />
                  {rebuildProgress.total > 0 && (
                    <span className='ml-2 text-xs'>
                      {rebuildProgress.current}/{rebuildProgress.total}
                    </span>
                  )}
                </>
              )}
              {rebuilt && (
                <span className='text-success ml-2'>
                  <FontAwesomeIcon icon={faCheck}></FontAwesomeIcon>
                </span>
              )}
            </button>
          </div>
          
          {rebuilding && (
            <div className='mt-3'>
              <div className='text-sm text-base-content/70 mb-1'>
                {rebuildProgress.status === 'starting' || rebuildProgress.status === 'scanning' || rebuildProgress.total === 0 ? (
                  'Scanning shot history files...'
                ) : (
                  `Processing shot history files (${rebuildProgress.current}/${rebuildProgress.total})`
                )}
              </div>
              <div className='bg-base-300 h-2 w-full overflow-hidden rounded'>
                <div
                  className={`h-full transition-all duration-300 ${
                    rebuildProgress.total === 0 ? 'bg-primary animate-pulse' : 'bg-primary'
                  }`}
                  style={{ 
                    width: rebuildProgress.total > 0 
                      ? `${(rebuildProgress.current / rebuildProgress.total) * 100}%`
                      : '30%'
                  }}
                />
              </div>
            </div>
          )}
        </div>
      </form>
    </>
  );
}
