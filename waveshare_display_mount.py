"""
FreeCAD Macro: Waveshare 1.43" Round AMOLED (ESP32-S3) Wall Mount v4
=====================================================================
Run in FreeCAD 0.21+ via Macro -> Macros -> Execute.

Design: Semi-circle cradle with back plate, tilted 65 deg.
        Half-tube arm merging solidly into cradle.
        Base plate with edge hooks for 1mm steel panel.
"""

import FreeCAD as App
import Part
import math

# ================================================================
#  PARAMETERS
# ================================================================

# --- Panel openings ---
OPENING_W    = 20.0
OPENING_H    = 28.0
OPENING_GAP  = 8.0
PANEL_T      = 1.0

# --- Base plate ---
BASE_W = OPENING_W * 2 + OPENING_GAP + 10.0   # ~58mm
BASE_H = OPENING_H + 6.0                        # ~34mm
BASE_T = 2.0

# --- Hooks ---
HOOK_W       = 3.0
HOOK_T       = 1.5
HOOK_SPAN    = 8.0
HOOK_GAP     = 1.2

# --- Half-tube arm ---
ARM_LEN      = 14.0
ARM_DIA      = 20.0
ARM_WALL     = 2.5

# --- Wire channel ---
WIRE_W       = 7.0
WIRE_H       = 4.0

# --- Display cradle ---
DISP_DIA     = 48.0    # display outer diameter, mm
DISP_T       = 12.0    # display module thickness, mm
CRADLE_WALL  = 2.5     # wall thickness around display, mm
CRADLE_BACK  = 2.5     # back plate thickness (display sits against this), mm
TILT_DEG     = 65.0

# Cradle total depth = display thickness + back plate
CRADLE_DEPTH = DISP_T + CRADLE_BACK   # 14.5mm

# ================================================================
#  HELPERS
# ================================================================

def fuse_all(shapes):
    result = shapes[0]
    for s in shapes[1:]:
        result = result.fuse(s)
    return result


def make_edge_hook(y_center, z_center, direction):
    """Simple edge hook with tooth for gripping 1mm steel panel."""
    parts_list = []

    if direction in ('left', 'right'):
        arm = Part.makeBox(
            HOOK_W, HOOK_T, HOOK_SPAN,
            App.Vector(-HOOK_W, y_center - HOOK_T / 2, z_center - HOOK_SPAN / 2),
        )
        tooth_offset = HOOK_GAP + PANEL_T
        if direction == 'left':
            tooth = Part.makeBox(
                HOOK_T, HOOK_T + tooth_offset, HOOK_SPAN,
                App.Vector(-HOOK_W, y_center - HOOK_T / 2 - tooth_offset, z_center - HOOK_SPAN / 2),
            )
        else:
            tooth = Part.makeBox(
                HOOK_T, HOOK_T + tooth_offset, HOOK_SPAN,
                App.Vector(-HOOK_W, y_center + HOOK_T / 2, z_center - HOOK_SPAN / 2),
            )
        parts_list = [arm, tooth]

    elif direction in ('top', 'bottom'):
        arm = Part.makeBox(
            HOOK_W, HOOK_SPAN, HOOK_T,
            App.Vector(-HOOK_W, y_center - HOOK_SPAN / 2, z_center - HOOK_T / 2),
        )
        tooth_offset = HOOK_GAP + PANEL_T
        if direction == 'top':
            tooth = Part.makeBox(
                HOOK_T, HOOK_SPAN, HOOK_T + tooth_offset,
                App.Vector(-HOOK_W, y_center - HOOK_SPAN / 2, z_center + HOOK_T / 2),
            )
        else:
            tooth = Part.makeBox(
                HOOK_T, HOOK_SPAN, HOOK_T + tooth_offset,
                App.Vector(-HOOK_W, y_center - HOOK_SPAN / 2, z_center - HOOK_T / 2 - tooth_offset),
            )
        parts_list = [arm, tooth]

    return fuse_all(parts_list)


# ================================================================
#  BUILD
# ================================================================

doc = App.newDocument("WaveshareMount_v4")
parts = []

# ------------------------------------------------------------------
# 1. BASE PLATE
# ------------------------------------------------------------------
base = Part.makeBox(
    BASE_T, BASE_W, BASE_H,
    App.Vector(0, -BASE_W / 2, -BASE_H / 2),
)
parts.append(base)

# ------------------------------------------------------------------
# 2. HOOKS (L-shaped clips, 3 per opening = 6 total)
# ------------------------------------------------------------------
# Hook dimensions from design sketch:
HK_ARM   = 2.0    # depth behind base plate (-X), mm
HK_LIP   = 2.5    # lip length folding back toward base, mm
HK_H     = 2.5    # hook height (perpendicular to gripped edge), mm
HK_GAP   = 1.0    # gap for 1mm steel plate
HK_W     = 2.5    # width along the edge, mm

left_open_cy  = -(OPENING_GAP / 2 + OPENING_W / 2)
right_open_cy = +(OPENING_GAP / 2 + OPENING_W / 2)

# Opening edges in base plate coordinates:
# Left opening: Y from (left_open_cy - 10) to (left_open_cy + 10), Z from -14 to +14
# Right opening: Y from (right_open_cy - 10) to (right_open_cy + 10), Z from -14 to +14
left_y  = left_open_cy     # center Y of left opening
right_y = right_open_cy    # center Y of right opening
top_z   = OPENING_H / 2    # top edge Z = +14
bot_z   = -OPENING_H / 2   # bottom edge Z = -14
left_edge_y  = left_open_cy - OPENING_W / 2    # left edge of left opening
right_edge_y = right_open_cy + OPENING_W / 2   # right edge of right opening

def make_hook_v2(y, z, edge_type):
    """
    Build an L-shaped hook at position (y, z) on the back of the base plate.
    edge_type: 'top', 'bottom', 'left', 'right' — which opening edge it grips.

    Hook profile (side view for top/bottom hooks):
      Base plate (X=0 to X=BASE_T)
      |
      |--- arm (2mm into -X) ---+
                                |  spine (connects arm to lip)
      gap (1mm for steel)       |
                                |
      |--- lip (2.5mm back) ----+
    """
    hook_parts = []

    if edge_type == 'top':
        # Grips top edge of opening — arm goes behind, lip folds down
        # Arm: behind base at top edge
        arm = Part.makeBox(HK_ARM, HK_W, HK_H,
            App.Vector(-HK_ARM, y - HK_W / 2, z))
        # Spine: vertical connector at far end
        spine_h = HK_H + HK_GAP + HK_H
        spine = Part.makeBox(1.0, HK_W, spine_h,
            App.Vector(-HK_ARM, y - HK_W / 2, z - HK_GAP - HK_H))
        # Lip: folds back toward base below the gap
        lip = Part.makeBox(HK_LIP, HK_W, HK_H,
            App.Vector(-HK_ARM, y - HK_W / 2, z - HK_GAP - HK_H))
        hook_parts = [arm, spine, lip]

    elif edge_type == 'bottom':
        # Grips bottom edge — arm goes behind, lip folds up
        arm = Part.makeBox(HK_ARM, HK_W, HK_H,
            App.Vector(-HK_ARM, y - HK_W / 2, z - HK_H))
        spine_h = HK_H + HK_GAP + HK_H
        spine = Part.makeBox(1.0, HK_W, spine_h,
            App.Vector(-HK_ARM, y - HK_W / 2, z))
        lip = Part.makeBox(HK_LIP, HK_W, HK_H,
            App.Vector(-HK_ARM, y - HK_W / 2, z + HK_GAP))
        hook_parts = [arm, spine, lip]

    elif edge_type == 'left':
        # Grips left vertical edge — arm goes behind, lip folds right
        arm = Part.makeBox(HK_ARM, HK_H, HK_W,
            App.Vector(-HK_ARM, y - HK_H, z - HK_W / 2))
        spine_w = HK_H + HK_GAP + HK_H
        spine = Part.makeBox(1.0, spine_w, HK_W,
            App.Vector(-HK_ARM, y, z - HK_W / 2))
        lip = Part.makeBox(HK_LIP, HK_H, HK_W,
            App.Vector(-HK_ARM, y + HK_GAP, z - HK_W / 2))
        hook_parts = [arm, spine, lip]

    elif edge_type == 'right':
        # Grips right vertical edge — arm goes behind, lip folds left
        arm = Part.makeBox(HK_ARM, HK_H, HK_W,
            App.Vector(-HK_ARM, y, z - HK_W / 2))
        spine_w = HK_H + HK_GAP + HK_H
        spine = Part.makeBox(1.0, spine_w, HK_W,
            App.Vector(-HK_ARM, y - spine_w + HK_H, z - HK_W / 2))
        lip = Part.makeBox(HK_LIP, HK_H, HK_W,
            App.Vector(-HK_ARM, y - HK_GAP - HK_H, z - HK_W / 2))
        hook_parts = [arm, spine, lip]

    return fuse_all(hook_parts)

# Left opening: top, bottom, left side
parts.append(make_hook_v2(left_y, top_z, 'top'))
parts.append(make_hook_v2(left_y, bot_z, 'bottom'))
parts.append(make_hook_v2(left_edge_y, 0, 'left'))

# Right opening: top, bottom, right side
parts.append(make_hook_v2(right_y, top_z, 'top'))
parts.append(make_hook_v2(right_y, bot_z, 'bottom'))
parts.append(make_hook_v2(right_edge_y, 0, 'right'))

# ------------------------------------------------------------------
# 3. HALF-TUBE ARM
# ------------------------------------------------------------------
arm_outer_r = ARM_DIA / 2
arm_inner_r = ARM_DIA / 2 - ARM_WALL

outer_cyl = Part.makeCylinder(
    arm_outer_r, ARM_LEN,
    App.Vector(BASE_T, 0, 0),
    App.Vector(1, 0, 0),
)
inner_cyl = Part.makeCylinder(
    arm_inner_r, ARM_LEN + 2,
    App.Vector(BASE_T - 1, 0, 0),
    App.Vector(1, 0, 0),
)
top_cut = Part.makeBox(
    ARM_LEN + 4, ARM_DIA + 4, arm_outer_r + 2,
    App.Vector(BASE_T - 2, -ARM_DIA / 2 - 2, 0),
)
arm_tube = outer_cyl.cut(inner_cyl).cut(top_cut)
parts.append(arm_tube)

# Flat cover plate on top of the half-tube arm (closes the open top)
# 2.5mm thick, clipped to the arm's circular profile so no overhang
ARM_COVER_T = 2.5
arm_cover_box = Part.makeBox(
    ARM_LEN, ARM_DIA, ARM_COVER_T,
    App.Vector(BASE_T, -ARM_DIA / 2, -ARM_COVER_T),
)
# Clip to arm's cylindrical profile — intersect with a cylinder matching the arm outer radius
arm_cover_clip = Part.makeCylinder(
    arm_outer_r, ARM_LEN,
    App.Vector(BASE_T, 0, 0),
    App.Vector(1, 0, 0),
)
arm_cover = arm_cover_box.common(arm_cover_clip)
parts.append(arm_cover)

# ------------------------------------------------------------------
# 4. DISPLAY CRADLE (semi-circle with solid back plate)
# ------------------------------------------------------------------
# Build at origin, axis along +X, then rotate and translate.
#
# Structure (side view, before rotation):
#   Back plate (solid disc, CRADLE_BACK thick) at X=0
#   Then display pocket (bore) from X=CRADLE_BACK to X=CRADLE_DEPTH
#   Total length = CRADLE_DEPTH = DISP_T + CRADLE_BACK

outer_r = DISP_DIA / 2 + CRADLE_WALL
inner_r = DISP_DIA / 2

# Full outer cylinder (the entire cradle shell)
outer = Part.makeCylinder(
    outer_r, CRADLE_DEPTH,
    App.Vector(0, 0, 0),
    App.Vector(1, 0, 0),
)

# Inner bore for display body - starts AFTER the back plate
# Only goes to depth = DISP_T (leaves CRADLE_BACK solid at X=0 end)
bore = Part.makeCylinder(
    inner_r, DISP_T + 1,    # slightly longer to ensure clean cut at open end
    App.Vector(CRADLE_BACK, 0, 0),
    App.Vector(1, 0, 0),
)

# Small wire hole through the back plate
wire_pass_r = max(WIRE_W, WIRE_H) / 2 + 0.5
wire_pass = Part.makeCylinder(
    wire_pass_r, CRADLE_DEPTH + 4,
    App.Vector(-2, 0, 0),
    App.Vector(1, 0, 0),
)

# Cut top half for semi-circle shape
cradle_top_cut = Part.makeBox(
    CRADLE_DEPTH + 4, outer_r * 2 + 4, outer_r + 2,
    App.Vector(-2, -outer_r - 2, 0),
)

# Vertical wire groove on back plate face (4mm wide, 1.5mm deep)
# Runs from wire hole center upward to the top edge (Z=0) only
# Does NOT extend below the wire hole — keeps the bottom closed
WIRE_GROOVE_W = 4.0    # width (Y), mm
WIRE_GROOVE_D = 1.5    # depth into back plate (X), mm
# Groove on back plate: vertical channel from center down toward bottom.
# In local coords: Z=0 is the flat top edge, Z=-outer_r is the bottom.
# Groove runs from near the top (Z=-2) down to near the bottom (Z=-(outer_r-CRADLE_WALL))
# keeping the very bottom closed (CRADLE_WALL margin from curved edge).
groove_top_z = -2.0                          # start just below the flat top edge
groove_bot_z = -(inner_r - 2.0)              # stop before the curved bottom wall
groove_height = groove_top_z - groove_bot_z   # total groove length
wire_groove = Part.makeBox(
    WIRE_GROOVE_D, WIRE_GROOVE_W, groove_height,
    App.Vector(CRADLE_BACK - WIRE_GROOVE_D, -WIRE_GROOVE_W / 2, groove_bot_z),
)

cradle = outer.cut(bore).cut(cradle_top_cut).cut(wire_groove)

# --- Tilt: top toward wall, bottom away ---
rot_deg = -(90.0 - TILT_DEG)   # -25 deg
cradle.rotate(
    App.Vector(0, 0, 0),
    App.Vector(0, 1, 0),
    rot_deg,
)

# Translate cradle so its back plate meets the arm tip
arm_tip_x = BASE_T + ARM_LEN
cradle.translate(App.Vector(arm_tip_x, 0, 0))
parts.append(cradle)

# ------------------------------------------------------------------
# 5. ARM-TO-CRADLE BRIDGE (external only, no material inside cradle)
# ------------------------------------------------------------------
# Extend the arm half-tube to overlap with the cradle's outer wall.
# Only adds material on the outside — the display pocket stays clear.

# Extend arm half-tube to overlap with cradle's outer back wall
bridge_len = 12.0
bridge_cyl = Part.makeCylinder(
    arm_outer_r, bridge_len,
    App.Vector(arm_tip_x - 2, 0, 0),
    App.Vector(1, 0, 0),
)
# Keep only bottom half
bridge_top_cut = Part.makeBox(
    bridge_len + 4, ARM_DIA + 4, arm_outer_r + 2,
    App.Vector(arm_tip_x - 4, -ARM_DIA / 2 - 2, 0),
)
bridge = bridge_cyl.cut(bridge_top_cut)

# Cut the cradle bore out of the bridge so it doesn't intrude into the display pocket
# Recreate the bore volume in the same tilted/translated position as the cradle
bridge_bore = Part.makeCylinder(
    inner_r, DISP_T + 4,
    App.Vector(CRADLE_BACK, 0, 0),
    App.Vector(1, 0, 0),
)
bridge_bore.rotate(App.Vector(0, 0, 0), App.Vector(0, 1, 0), rot_deg)
bridge_bore.translate(App.Vector(arm_tip_x, 0, 0))
bridge = bridge.cut(bridge_bore)

parts.append(bridge)

# ------------------------------------------------------------------
# 6. WIRE CHANNEL
# ------------------------------------------------------------------
wire_cut_len = arm_tip_x + CRADLE_DEPTH + 5
wire_channel = Part.makeBox(
    wire_cut_len, WIRE_W, WIRE_H,
    App.Vector(-1, -WIRE_W / 2, -arm_outer_r + ARM_WALL),
)
wire_rear = Part.makeBox(
    BASE_T + 2, WIRE_W + 2, WIRE_H + 2,
    App.Vector(-1, -WIRE_W / 2 - 1, -arm_outer_r + ARM_WALL - 1),
)
wire_cut = wire_channel.fuse(wire_rear)

# Rectangular groove on the BACK of the base plate for wiring
# Matches manual placement: X=0, Y=4.5, Z=-8, size 1.5 x 15 x 5
base_groove = Part.makeBox(
    1.5, 15.0, 5.0,
    App.Vector(0, 4.5, -8.0),
)
wire_cut = wire_cut.fuse(base_groove)

# ------------------------------------------------------------------
# 7. FINAL BOOLEAN
# ------------------------------------------------------------------
solid = fuse_all(parts)
solid = solid.cut(wire_cut)
solid = solid.removeSplitter()

# ------------------------------------------------------------------
# 8. ADD TO DOCUMENT
# ------------------------------------------------------------------
mount = doc.addObject("Part::Feature", "DisplayMount_v4")
mount.Shape = solid
doc.recompute()

try:
    import FreeCADGui as Gui
    Gui.activeDocument().activeView().viewIsometric()
    Gui.SendMsgToActiveView("ViewFit")
except Exception:
    pass

# ------------------------------------------------------------------
# SUMMARY
# ------------------------------------------------------------------
bb = solid.BoundBox
print("=" * 60)
print("  Waveshare 1.43\" AMOLED Mount v4 - BUILD COMPLETE")
print("=" * 60)
print(f"  Bounding box:   {bb.XLength:.1f} x {bb.YLength:.1f} x {bb.ZLength:.1f} mm")
print(f"  Volume:          {solid.Volume:.1f} mm3")
print(f"  Base plate:      {BASE_W:.0f} x {BASE_H:.0f} x {BASE_T:.0f} mm")
print(f"  Arm:             {ARM_LEN:.0f} mm, {ARM_DIA:.0f} mm dia half-tube")
print(f"  Cradle:          {DISP_DIA:.0f} mm bore, {CRADLE_DEPTH:.1f} mm deep")
print(f"    Back plate:    {CRADLE_BACK:.1f} mm solid")
print(f"    Display pocket:{DISP_T:.0f} mm deep")
print(f"  Tilt:            {TILT_DEG:.0f} deg (top toward wall)")
print(f"  Hooks:           {HOOK_W:.0f} mm wide for {PANEL_T:.0f} mm steel")
print("=" * 60)
