import { useState } from 'preact/hooks';
import { faTrashCan } from '@fortawesome/free-solid-svg-icons/faTrashCan';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import homekitImage from '../../assets/homekit.png';

function GitLabBlogCard({ formData, onChange }) {
  const [testResult, setTestResult] = useState(null);
  const [testing, setTesting] = useState(false);
  const [publishResult, setPublishResult] = useState(null);
  const [publishing, setPublishing] = useState(false);

  const handleTest = async () => {
    setTesting(true);
    setTestResult(null);
    try {
      const res = await fetch('/api/gitlab-blog/test', { method: 'POST' });
      const data = await res.json();
      setTestResult(data);
    } catch {
      setTestResult({ success: false, message: 'Request failed - is the device connected?' });
    } finally {
      setTesting(false);
    }
  };

  const handlePublish = async () => {
    setPublishing(true);
    setPublishResult(null);
    try {
      const res = await fetch('/api/gitlab-blog/publish', { method: 'POST' });
      const data = await res.json();
      setPublishResult(data);
    } catch {
      setPublishResult({ success: false, message: 'Request failed - is the device connected?' });
    } finally {
      setPublishing(false);
    }
  };

  return (
    <div className='bg-base-200 rounded-lg p-4'>
      <div className='flex items-center justify-between'>
        <span className='text-xl font-medium'>GitLab Blog</span>
        <input
          id='gitlabBlogActive'
          name='gitlabBlogActive'
          value='gitlabBlogActive'
          type='checkbox'
          className='toggle toggle-primary'
          checked={!!formData.gitlabBlogActive}
          onChange={onChange('gitlabBlogActive')}
          aria-label='Enable GitLab Blog'
        />
      </div>
      {formData.gitlabBlogActive && (
        <div className='border-base-300 mt-4 space-y-4 border-t pt-4'>
          <p className='text-sm opacity-70'>
            Automatically publish shot data to your GitLab Pages blog after each brew. Creates a
            markdown post with shot summary and chart data.
          </p>
          <div className='form-control'>
            <label htmlFor='gitlabBlogHost' className='mb-2 block text-sm font-medium'>
              GitLab Host
            </label>
            <input
              id='gitlabBlogHost'
              name='gitlabBlogHost'
              type='text'
              className='input input-bordered w-full'
              placeholder='gitlab.com'
              value={formData.gitlabBlogHost}
              onChange={onChange('gitlabBlogHost')}
            />
          </div>
          <div className='form-control'>
            <label htmlFor='gitlabBlogProjectId' className='mb-2 block text-sm font-medium'>
              Project ID or Path
            </label>
            <input
              id='gitlabBlogProjectId'
              name='gitlabBlogProjectId'
              type='text'
              className='input input-bordered w-full'
              placeholder='pministry/pministry.gitlab.io'
              value={formData.gitlabBlogProjectId}
              onChange={onChange('gitlabBlogProjectId')}
            />
          </div>
          <div className='form-control'>
            <label htmlFor='gitlabBlogToken' className='mb-2 block text-sm font-medium'>
              Personal Access Token
            </label>
            <input
              id='gitlabBlogToken'
              name='gitlabBlogToken'
              type='password'
              className='input input-bordered w-full'
              placeholder='glpat-...'
              value={formData.gitlabBlogToken}
              onChange={onChange('gitlabBlogToken')}
            />
          </div>
          <div className='form-control'>
            <label htmlFor='gitlabBlogPath' className='mb-2 block text-sm font-medium'>
              Content Path
            </label>
            <input
              id='gitlabBlogPath'
              name='gitlabBlogPath'
              type='text'
              className='input input-bordered w-full'
              placeholder='src/content/shots'
              value={formData.gitlabBlogPath}
              onChange={onChange('gitlabBlogPath')}
            />
          </div>
          <div className='flex flex-wrap items-center gap-3'>
            <button
              type='button'
              className='btn btn-outline btn-sm'
              onClick={handleTest}
              disabled={testing}
            >
              {testing ? (
                <>
                  <span className='loading loading-spinner loading-xs' />
                  Testing...
                </>
              ) : (
                'Test Connection'
              )}
            </button>
            <button
              type='button'
              className='btn btn-outline btn-sm'
              onClick={handlePublish}
              disabled={publishing}
            >
              {publishing ? (
                <>
                  <span className='loading loading-spinner loading-xs' />
                  Publishing...
                </>
              ) : (
                'Publish Last Shot'
              )}
            </button>
            {testResult && (
              <span
                className={`text-sm font-medium ${testResult.success ? 'text-success' : 'text-error'}`}
              >
                {testResult.message}
              </span>
            )}
            {publishResult && (
              <span
                className={`text-sm font-medium ${publishResult.success ? 'text-success' : 'text-error'}`}
              >
                {publishResult.message}
              </span>
            )}
          </div>
        </div>
      )}
    </div>
  );
}

export function PluginCard({
  formData,
  onChange,
  autowakeupSchedules,
  addAutoWakeupSchedule,
  removeAutoWakeupSchedule,
  updateAutoWakeupTime,
  updateAutoWakeupDay,
}) {
  return (
    <div className='space-y-4'>
      <div className='bg-base-200 rounded-lg p-4'>
        <div className='flex items-center justify-between'>
          <span className='text-xl font-medium'>Automatic Wakeup Schedule</span>
          <input
            id='autowakeupEnabled'
            name='autowakeupEnabled'
            value='autowakeupEnabled'
            type='checkbox'
            className='toggle toggle-primary'
            checked={!!formData.autowakeupEnabled}
            onChange={onChange('autowakeupEnabled')}
            aria-label='Enable Auto Wakeup'
          />
        </div>
        {formData.autowakeupEnabled && (
          <div className='border-base-300 mt-4 space-y-4 border-t pt-4'>
            <p className='text-sm opacity-70'>
              Automatically switch to brew mode at specified time(s) of day.
            </p>
            <div className='form-control'>
              <label className='mb-2 block text-sm font-medium'>Auto Wakeup Schedule</label>
              <div className='space-y-2'>
                {autowakeupSchedules?.map((schedule, scheduleIndex) => (
                  <div key={scheduleIndex} className='flex flex-wrap items-center gap-1'>
                    {/* Time input */}
                    <input
                      type='time'
                      className='input input-bordered input-sm w-auto min-w-0 pr-6'
                      value={schedule.time}
                      onChange={e => updateAutoWakeupTime(scheduleIndex, e.target.value)}
                      disabled={!formData.autowakeupEnabled}
                    />

                    {/* Days toggle buttons */}
                    <div className='join' role='group' aria-label='Days of week selection'>
                      {['M', 'T', 'W', 'T', 'F', 'S', 'S'].map((dayLabel, dayIndex) => (
                        <button
                          key={dayIndex}
                          type='button'
                          className={`join-item btn btn-xs ${schedule.days[dayIndex] ? 'btn-primary' : 'btn-outline'}`}
                          onClick={() =>
                            updateAutoWakeupDay(scheduleIndex, dayIndex, !schedule.days[dayIndex])
                          }
                          disabled={!formData.autowakeupEnabled}
                          aria-pressed={schedule.days[dayIndex]}
                          aria-label={
                            [
                              'Monday',
                              'Tuesday',
                              'Wednesday',
                              'Thursday',
                              'Friday',
                              'Saturday',
                              'Sunday',
                            ][dayIndex]
                          }
                          title={
                            [
                              'Monday',
                              'Tuesday',
                              'Wednesday',
                              'Thursday',
                              'Friday',
                              'Saturday',
                              'Sunday',
                            ][dayIndex]
                          }
                        >
                          {dayLabel}
                        </button>
                      ))}
                    </div>

                    {/* Delete button */}
                    {autowakeupSchedules.length > 1 ? (
                      <button
                        type='button'
                        onClick={() => removeAutoWakeupSchedule(scheduleIndex)}
                        className='btn btn-ghost btn-xs'
                        disabled={!formData.autowakeupEnabled}
                        title='Delete this schedule'
                      >
                        <FontAwesomeIcon icon={faTrashCan} className='text-xs' />
                      </button>
                    ) : (
                      <div
                        className='btn btn-ghost btn-xs cursor-not-allowed opacity-30'
                        title='Cannot delete the last schedule'
                      >
                        <FontAwesomeIcon icon={faTrashCan} className='text-xs' />
                      </div>
                    )}
                  </div>
                ))}
                <button
                  type='button'
                  onClick={addAutoWakeupSchedule}
                  className='btn btn-primary btn-sm'
                  disabled={!formData.autowakeupEnabled}
                >
                  Add Schedule
                </button>
              </div>
            </div>
          </div>
        )}
      </div>

      <div className='bg-base-200 rounded-lg p-4'>
        <div className='flex items-center justify-between'>
          <span className='text-xl font-medium'>HomeKit</span>
          <input
            id='homekit'
            name='homekit'
            value='homekit'
            type='checkbox'
            className='toggle toggle-primary'
            checked={!!formData.homekit}
            onChange={onChange('homekit')}
            aria-label='Enable HomeKit'
          />
        </div>
        {formData.homekit && (
          <div className='border-base-300 mt-4 flex flex-col items-center justify-center gap-4 border-t pt-4'>
            <img src={homekitImage} alt='HomeKit Setup Code' />
            <p className='text-center'>
              Open the Home app on your iOS device, select Add Accessory, and enter the setup code
              shown above.
            </p>
          </div>
        )}
      </div>

      <div className='bg-base-200 rounded-lg p-4'>
        <div className='flex items-center justify-between'>
          <span className='text-xl font-medium'>Boiler Refill Plugin</span>
          <input
            id='boilerFillActive'
            name='boilerFillActive'
            value='boilerFillActive'
            type='checkbox'
            className='toggle toggle-primary'
            checked={!!formData.boilerFillActive}
            onChange={onChange('boilerFillActive')}
            aria-label='Enable Boiler Refill'
          />
        </div>
        {formData.boilerFillActive && (
          <div className='border-base-300 mt-4 grid grid-cols-2 gap-4 border-t pt-4'>
            <div className='form-control'>
              <label htmlFor='startupFillTime' className='mb-2 block text-sm font-medium'>
                On startup (s)
              </label>
              <input
                id='startupFillTime'
                name='startupFillTime'
                type='number'
                className='input input-bordered w-full'
                placeholder='0'
                value={formData.startupFillTime}
                onChange={onChange('startupFillTime')}
              />
            </div>
            <div className='form-control'>
              <label htmlFor='steamFillTime' className='mb-2 block text-sm font-medium'>
                On steam deactivate (s)
              </label>
              <input
                id='steamFillTime'
                name='steamFillTime'
                type='number'
                className='input input-bordered w-full'
                placeholder='0'
                value={formData.steamFillTime}
                onChange={onChange('steamFillTime')}
              />
            </div>
          </div>
        )}
      </div>

      <div className='bg-base-200 rounded-lg p-4'>
        <div className='flex items-center justify-between'>
          <span className='text-xl font-medium'>Smart Grind Plugin</span>
          <input
            id='smartGrindActive'
            name='smartGrindActive'
            value='smartGrindActive'
            type='checkbox'
            className='toggle toggle-primary'
            checked={!!formData.smartGrindActive}
            onChange={onChange('smartGrindActive')}
            aria-label='Enable Smart Grind'
          />
        </div>
        {formData.smartGrindActive && (
          <div className='border-base-300 mt-4 space-y-4 border-t pt-4'>
            <p className='text-sm opacity-70'>
              This feature controls a Tasmota Plug to turn off your grinder after the target has
              been reached.
            </p>
            <div className='form-control'>
              <label htmlFor='smartGrindIp' className='mb-2 block text-sm font-medium'>
                Tasmota IP
              </label>
              <input
                id='smartGrindIp'
                name='smartGrindIp'
                type='text'
                className='input input-bordered w-full'
                placeholder='0'
                value={formData.smartGrindIp}
                onChange={onChange('smartGrindIp')}
              />
            </div>
            <div className='form-control'>
              <label htmlFor='smartGrindMode' className='mb-2 block text-sm font-medium'>
                Mode
              </label>
              <select
                id='smartGrindMode'
                name='smartGrindMode'
                className='select select-bordered w-full'
                onChange={onChange('smartGrindMode')}
              >
                <option value='0' selected={formData.smartGrindMode?.toString() === '0'}>
                  Turn off at target
                </option>
                <option value='1' selected={formData.smartGrindMode?.toString() === '1'}>
                  Toggle off and on at target
                </option>
                <option value='2' selected={formData.smartGrindMode?.toString() === '2'}>
                  Turn on at start, off at target
                </option>
              </select>
            </div>
          </div>
        )}
      </div>

      <GitLabBlogCard formData={formData} onChange={onChange} />

      <div className='bg-base-200 rounded-lg p-4'>
        <div className='flex items-center justify-between'>
          <span className='text-xl font-medium'>Home Assistant over MQTT (Deprecated)</span>
          <input
            id='homeAssistant'
            name='homeAssistant'
            value='homeAssistant'
            type='checkbox'
            className='toggle toggle-primary'
            checked={!!formData.homeAssistant}
            onChange={onChange('homeAssistant')}
            aria-label='Enable Home Assistant'
          />
        </div>
        {formData.homeAssistant && (
          <div className='border-base-300 mt-4 space-y-4 border-t pt-4'>
            <p className='text-sm opacity-70'>
              This feature allows connection to a Home Assistant or MQTT installation and push the
              current state. This feature is deprecated for usage with Home Assistant. Please see
              the{' '}
              <a
                href='https://github.com/gaggimate/ha-integration'
                target='_blank'
                rel='noreferrer'
              >
                Home Assistant Integration
              </a>{' '}
              for a more up-to-date solution.
            </p>
            <div className='form-control'>
              <label htmlFor='haIP' className='mb-2 block text-sm font-medium'>
                MQTT IP
              </label>
              <input
                id='haIP'
                name='haIP'
                type='text'
                className='input input-bordered w-full'
                placeholder='0'
                value={formData.haIP}
                onChange={onChange('haIP')}
              />
            </div>

            <div className='form-control'>
              <label htmlFor='haPort' className='mb-2 block text-sm font-medium'>
                MQTT Port
              </label>
              <input
                id='haPort'
                name='haPort'
                type='number'
                className='input input-bordered w-full'
                placeholder='0'
                value={formData.haPort}
                onChange={onChange('haPort')}
              />
            </div>

            <div className='form-control'>
              <label htmlFor='haUser' className='mb-2 block text-sm font-medium'>
                MQTT User
              </label>
              <input
                id='haUser'
                name='haUser'
                type='text'
                className='input input-bordered w-full'
                placeholder='user'
                value={formData.haUser}
                onChange={onChange('haUser')}
              />
            </div>

            <div className='form-control'>
              <label htmlFor='haPassword' className='mb-2 block text-sm font-medium'>
                MQTT Password
              </label>
              <input
                id='haPassword'
                name='haPassword'
                type='password'
                className='input input-bordered w-full'
                placeholder='password'
                value={formData.haPassword}
                onChange={onChange('haPassword')}
              />
            </div>
            <div className='form-control'>
              <label htmlFor='haTopic' className='mb-2 block text-sm font-medium'>
                Home Assistant Discovery Topic
              </label>
              <input
                id='haTopic'
                name='haTopic'
                type='text'
                className='input input-bordered w-full'
                value={formData.haTopic}
                onChange={onChange('haTopic')}
              />
            </div>
          </div>
        )}
      </div>
    </div>
  );
}
