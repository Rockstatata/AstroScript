mission ModeAbort launch
{
  mode MissionState
  {
    trajectory 1.
    trajectory 2.
    fallback 0.
  }

  telemetry count code := 1.
  transmit code.
  abort.
}
success
