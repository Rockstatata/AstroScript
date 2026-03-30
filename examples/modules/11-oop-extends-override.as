mission MissionOOP launch
{
  module BaseModule
  {
    public telemetry count id := 0.

    command BaseModule(count initial) : voidspace
    {
      this.id := initial.
    }

    command base_id() : count
    {
      back this.id.
    }

    command add_task(count incoming) : count
    {
      this.id := this.id add incoming.
      back this.id.
    }
  }

  module ChildModule extends BaseModule
  {
    public telemetry count emergencyBoost := 2.

    command ChildModule(count initial) : voidspace
    {
      this.id := initial.
    }

    override command add_task(count incoming) : count
    {
      back super.add_task(incoming add this.emergencyBoost).
    }
  }

  deploy ChildModule ship(10).
  transmit ship.add_task(3).
  transmit ship.id.

  telemetry ChildModule backup := new ChildModule(20).
  transmit backup.add_task(1).
}
success
