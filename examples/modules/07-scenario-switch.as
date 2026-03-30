mission ScenarioSwitch launch
{
  telemetry count code := 2.

  scenario (code)
  {
    trajectory 1:
      transmit "case 1".
      stage_sep.
    trajectory 2:
      transmit "case 2".
      stage_sep.
    fallback:
      transmit "default".
  }
}
success
