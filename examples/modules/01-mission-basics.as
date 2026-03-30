mission Apollo launch
{
  telemetry count astronauts := 3.

  verify (astronauts > 0)
  {
    transmit "Mission ready".
  }

  orbit (astronauts > 0)
  {
    transmit astronauts.
    astronauts := astronauts minus 1.
  }
}
success
