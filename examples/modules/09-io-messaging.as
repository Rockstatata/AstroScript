mission IOMessaging launch
{
  telemetry symbol txt := "hello".
  telemetry count val.

  transmit txt.
  broadcast "broadcast channel".
  alarm "alarm channel".
  receive val.
  transmit val.
}
success
