mission NestedScenarioLoops launch
{
  telemetry count lane.
  telemetry count scenarioMode := 2.

  orbit times (lane : 0 : 4)
  {
    scenario (scenarioMode)
    {
      trajectory 1:
        transmit lane.
        stage_sep.
      trajectory 2:
        verify (lane mod 2 == 0)
        {
          transmit lane mul 10.
        }
        otherwise
        {
          transmit lane mul 100.
        }
        stage_sep.
      fallback:
        transmit -1.
    }
  }
}
success
