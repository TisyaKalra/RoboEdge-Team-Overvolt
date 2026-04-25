# Control System Summary

The robot uses differential drive control with adaptive input mapping.

Key features:
- Deadzone filtering for stable neutral input
- Exponential mapping for fine control
- Software ramping to prevent current spikes
- IMU-based tilt correction using MPU6050

Modes:
- Manual Mode (RC control)
- Stabilized Mode (sensor-assisted control)

This architecture allows smooth operation across racing, obstacle, and soccer scenarios.
