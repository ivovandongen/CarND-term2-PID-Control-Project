# PID controller
Part of the Self-Driving Car nanodegree. Starter code and data from 
[Udacity](https://github.com/udacity/CarND-PID-Control-Project).

## Build
- `#>` `git submodule update --init --recursive`
- `#>` `mkdir build && cd build && cmake .. -DGIT_SUBMODULE=ON && make`

# Run

- `#>` `./pid`

# Run with Twiddle optimisation

- `#>` `./pid twiddle`


## Tuning of the hyperparameters

Initially I started out with the hyper-parameters as shown in the course (`P:0.2, D:0.004, I:3.0`), 
this resulted in a highly unstable course and did not let the car navigate the track
safely. Tuning the parameters manually was quite fast. Tuning the P parameter controls
the rate of oscillation. I had to tune it down a bit so it would approach more gradually.
The D parameter tunes the way the controller anticipates the needed corrections and avoids
overshoot, converging with less oscillation. The I parameter deals with systematic bias or drift.
The initial value was sufficient here, but twiddle raised it quite a bit. I ended up with
`P:0.1, D:0.001, I:3.0` after manual tuning.

In order for the car to be able to drive safely around the track, I tuned the throttle to be related
to the speed and the steering angle. Higher speeds and greater angles would lower the throttle or actively
break when necessary, giving the car the opportunity to drive at higher speeds when going straight, but break
for corners.

I then decided to see if optimizing the parameters further using [twiddle](blob/master/src/twiddle.hpp)
 would result in an even more stable course. Tweaking twiddle itself was fairly labour intensive,
choosing the correct number of telemetry events to process in each run and the amount
of telemetry events to use for warming up took quite a few iterations. Eventually it settled on
`P:0.45988, D:0.00323188, I:14.2266`. For this the throttle needed a little more tuning so the car
woudln't start actively breaking under a certain threshold as it would start to drive backwards
(the speed is always positive).


