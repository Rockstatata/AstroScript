mission NestedModuleMethods launch
{
  module Engine
  {
    public telemetry count power := 0.

    command Engine(count seed) : voidspace
    {
      this.power := seed.
    }

    command calibrate(count delta) : count
    {
      this.power := this.power add delta.
      back this.power.
    }

    command ignite(count level) : count
    {
      telemetry count tuned := this.calibrate(level).
      back tuned mul 2.
    }
  }

  deploy Engine e(5).
  transmit e.ignite(3).
  transmit e.calibrate(2).
  transmit e.power.
}
success
