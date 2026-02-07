# Servo Mapping Diagnostic Guide

## Physical Layout (from calibration.cpp)

```
       USB
        |
        ▼
   | \ ___   ________   ____/ |
   |(2)___](0)      (1)[___(3)|  ← Front
   |__/    |   KAME   |    \__|
           |          |
   | \____ |          | ____/ |
   |(6)___](4)______(5)[___(7)|  ← Back
   |__/                    \__|
```

## Leg to Servo Mapping

### Front Left Leg:
- **Servo 0** (PCA Channel 0) = Horizontal/Rotational servo
- **Servo 2** (PCA Channel 2) = Vertical/Height servo

### Front Right Leg:
- **Servo 1** (PCA Channel 1) = Horizontal/Rotational servo
- **Servo 3** (PCA Channel 3) = Vertical/Height servo

### Back Left Leg:
- **Servo 4** (PCA Channel 4) = Horizontal/Rotational servo
- **Servo 6** (PCA Channel 6) = Vertical/Height servo

### Back Right Leg:
- **Servo 5** (PCA Channel 5) = Horizontal/Rotational servo
- **Servo 7** (PCA Channel 7) = Vertical/Height servo

## Walking Gait Pattern

### Forward Walk (`walk()` function):
- **Always moving**: Servos 0, 1, 4, 5 (all horizontal servos)
- **Alternating leg pairs**:
  - **Side 0**: Servos 3 & 6 lift (Front Right + Back Left)
  - **Side 1**: Servos 2 & 7 lift (Front Left + Back Right)

This creates a **diagonal gait** - opposite legs lift together (trot gait).

### Backward Walk:
- Same pattern but with inverted phases

## Home Position Check

When `home()` is called, servos should be at:
- Servo 0: 110° (90+20) - Front Left horizontal (spread out)
- Servo 1: 70° (90-20) - Front Right horizontal (spread out)
- Servo 2: 55° (90-35) - Front Left vertical (lowered)
- Servo 3: 125° (90+35) - Front Right vertical (raised)
- Servo 4: 70° (90-20) - Back Left horizontal (spread out)
- Servo 5: 110° (90+20) - Back Right horizontal (spread out)
- Servo 6: 125° (90+35) - Back Left vertical (raised)
- Servo 7: 55° (90-35) - Back Right vertical (lowered)

## Troubleshooting Checklist

### 1. Check Physical Connections
Verify servos are connected to correct PCA9685 channels:
- [ ] Servo physically on Front Left → PCA Ch 0 (horizontal) & Ch 2 (vertical)
- [ ] Servo physically on Front Right → PCA Ch 1 (horizontal) & Ch 3 (vertical)
- [ ] Servo physically on Back Left → PCA Ch 4 (horizontal) & Ch 6 (vertical)
- [ ] Servo physically on Back Right → PCA Ch 5 (horizontal) & Ch 7 (vertical)

### 2. Check Servo Directions
In `home()` position, verify:
- [ ] Front Left horizontal (0) should spread OUTWARD
- [ ] Front Right horizontal (1) should spread OUTWARD
- [ ] Front Left vertical (2) should be LOWERED
- [ ] Front Right vertical (3) should be RAISED
- [ ] Back Left horizontal (4) should spread OUTWARD
- [ ] Back Right horizontal (5) should spread OUTWARD
- [ ] Back Left vertical (6) should be RAISED
- [ ] Back Right vertical (7) should be LOWERED

### 3. Common Issues

**Problem**: Robot tips over or legs cross
- **Check**: Horizontal servos (0,1,4,5) may be reversed or swapped
- **Fix**: Use `reverseServo()` function or swap physical connections

**Problem**: Robot doesn't lift legs when walking
- **Check**: Vertical servos (2,3,6,7) may be on wrong channels
- **Fix**: Verify vertical servos are on channels 2,3,6,7

**Problem**: Robot moves diagonally instead of forward
- **Check**: Leg pairs may be swapped (Front/Back or Left/Right)
- **Fix**: Verify all 4 legs match the diagram above

**Problem**: Walking is jerky or unbalanced
- **Check**: Calibration values may be wrong for servos 2,3,6,7
- **Fix**: Recalibrate using calibration web interface

### 4. Test Individual Servos

Test each servo individually by calling:
```cpp
robot.setServo(servo_id, 90);  // Center position
robot.setServo(servo_id, 0);   // Minimum
robot.setServo(servo_id, 180); // Maximum
```

Verify each servo moves in expected direction.

### 5. Verify Walking Sequence

When walking forward, you should see:
1. Front Right leg (servo 3) and Back Left leg (servo 6) lift together
2. Then Front Left leg (servo 2) and Back Right leg (servo 7) lift together
3. This alternates smoothly

If legs on the same side lift together instead of opposite legs, servos may be swapped.

## Expected Behavior Summary

✅ **Correct**: Diagonal gait - opposite legs lift together  
❌ **Wrong**: Same-side legs lift together  
❌ **Wrong**: All legs lift at once  
❌ **Wrong**: Only front or only back legs lift

