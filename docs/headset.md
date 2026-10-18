# Gled in a headset

A pupil can render its scene into a VR headset through OpenXR. The headset
rides the pupil's camera base, the controllers move and scale the scene, pick
and shoot along a laser, and work the pupil's overlay on a panel above the
left hand. This was developed with a Meta Quest 3 streamed from a Linux
desktop (X11, NVIDIA) by WiVRn, but nothing in it is specific to them.

## Building

The headset code is in `GledCore/Pupils/PupilXr.{h,cxx}` and is built when
`pkg-config openxr` finds the OpenXR loader and headers (on Fedora,
`openxr-devel`). Without them gled builds as before, and starting a session
prints that gled was built without OpenXR.

`PupilXr` uses the `XR_KHR_opengl_enable` extension with the Xlib binding, on
the pupil's own GL context, so the pupil must run under X11.

## Running

The OpenXR loader finds the runtime from the system's active runtime, or from
`XR_RUNTIME_JSON` when that is set; for WiVRn installed from Flathub that is
the `openxr_wivrn.json` inside the Flatpak's `share/openxr/1`.

- `GLED_XR=1` starts a session for the first pupil that is drawn.
- `PupilInfo` has the buttons **XrStart** and **XrStop**, which start and end
  the session of that pupil at any time.

While the session runs, the pupil's window shows the middle of the left eye.
It ignores the mouse, whose picks would not match the picture; keys still go
to the pupil.

Environment variables, read when a session starts:

| Variable | Meaning |
|---|---|
| `GLED_XR` | start a session for the first pupil drawn |
| `GLED_XR_SCALE` | metres per gled unit (default 0.2) |
| `GLED_XR_FLY` | flying acceleration of a fully pushed stick, m/s² (default 3) |
| `GLED_XR_DESKTOP` | keep the window's own camera view instead of the mirror |

## Riding the camera base

Gled's camera is a camera base, a node in the scene, with the camera's own
transform on top of it. In the headset, the head's tracked pose takes the
place of the camera's transform, so walking around the room is that
transform. The headset rides the camera base, skipping any Camera nodes above
it, raised along its up axis to the camera's height. Without a camera base it
rides the camera, levelled.

The room stands on a platform carried by the camera base, `XrRig` in
`GledCore/RnrBase`. Your place relative to the base, in the base's own frame,
stays fixed while the base moves and turns. It changes only when you walk,
fly, grab, or press A.

`PupilInfo::XrTilt` says what happens when the camera base tilts:

- **Ride** (the default): the base's tilts reach the head. The base's up
  stays the room's up and the world tilts around you, as in a car or on a
  roller coaster.
- **Level**: the tilts are undone about the base's place. The base's up of
  the moment Level was chosen stays up, and only its position and heading
  reach you.

It can be changed in the GUI or from a macro at any time and takes effect at
the next frame.

## Controls

For Oculus Touch controllers:

| Control | Action |
|---|---|
| grip, one hand | carry the scene with the hand, kept level |
| grips, both hands | also turn and scale the scene by the hands' distance |
| A, short press | with a spirit: recenter; without one: realign, turning the scene about the head so the camera base faces the gaze |
| A, held 0.6 s | recenter: the camera point at the head, facing the gaze, at the starting scale |
| right trigger | the laser picks, as mouse button 1 |
| B | as mouse button 3, which shoots in Tringula |
| X | show or hide the overlay panel above the left hand |
| left menu button | end the session |
| sticks | to the pupil's spirit, or fly |

**Sticks.** When the pupil has an event handler (a spirit), the sticks go to
it through `PupilInfo`; Tringula's `TringuCam` reads them like its keys: left
y forward and back, left x sideways, right x turns, right y up and down.
Without a spirit the sticks fly the platform: left y accelerates along the
gaze and left x strafes, both kept level; right x turns about the head; right
y moves down when pushed.

**The laser** comes from the right controller. On the overlay panel it works
the overlay's widgets as the mouse does; elsewhere its events go to the
pupil's normal event delivery, with the laser as the mouse ray.

## Demos

- `Var1/demos/tringula.C` — drive and shoot over Tringula with the sticks.
- `Geom1/demos/WS_demo.C(lift)` — set the travelling sphere as the camera
  base and start the eventor to ride the weaver symbol; `lift` waves the seed
  out of its plane so that it rolls. Under `GLED_XR` the eventor beats every
  11 ms, about once per headset frame.
- `Geom1/demos/coaster.C(mpu, flight)` — a roller coaster with a banked track
  in a landscape. The cart's seat is the camera base. `mpu` is metres per gled
  unit, matching `GLED_XR_SCALE`; with `flight` the cart is banked by the felt
  force as it rides (`WSWalker`'s flight mode) instead of by the track.

A simulation that moves the camera base should beat about as often as the
headset draws (90 Hz on a Quest 3): the headset renders every frame from the
lenses' current state, and a slower beat shows as a jerky ride.

## Tests

`test/regress/xr_rig.C` checks `XrRig` without a headset, and
`test/regress/ws_motion.C` the motion along a `WSSeed` and the walker's
banking; both run under `saturn`. See `test/README.md`.
