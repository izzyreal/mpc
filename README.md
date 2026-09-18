# mpc
mpc2000xl emulator static library

## Rotary pointer gestures

Shift + 0 opens VMPC-specific options. Scroll to **Rotary control drag** to choose
**VERTICAL** (the default), **CIRCULAR**, or **BY POSITION** for the DATA wheel,
MAIN VOLUME and REC GAIN. The choice is saved with the other VMPC settings.
BY POSITION uses circular dragging when contact starts in the outer half of the
control's radius, and vertical dragging near the center. The selected mode lasts
until release, even if the setting changes during a drag. Clockwise movement
increases the value without jumping to the initial pointer angle. The DATA wheel
tracks 100 steps per revolution before modifier/multitouch acceleration; the pots
follow their 306-degree sweep and stop at their limits. Scroll input is unchanged.

Hosts opt in with `GestureEvent::Movement::RotaryDrag` on BEGIN/REPEAT, UPDATE and
END. Supply component-relative `normX`/`normY` (unclamped during capture), a stable
source index, and upward pixel motion in `continuousDelta` for UPDATE. Forward
horizontal-only motion too. The core selects the drag mode, tracks angles and
clears active gestures on focus loss. Existing Relative input remains supported.

MPC® and Akai Professional® are a registered trademarks of inMusic Brands. Inc. This emulator is not affiliated with inMusic and use of the MPC® and Akai Professional® names has not been authorized, sponsored or otherwise approved by inMusic.
