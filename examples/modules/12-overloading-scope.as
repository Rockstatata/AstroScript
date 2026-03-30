mission OverloadingScope launch
{
  command route(count normal) : count
  {
    back normal add 1.
  }

  command route(count normal, count emergency) : count
  {
    back normal add emergency.
  }

  module MissionQueue
  {
    public telemetry count backlog := 0.

    command MissionQueue(count seed) : voidspace
    {
      this.backlog := seed.
    }

    command enqueue(count tasks) : count
    {
      this.backlog := this.backlog add tasks.
      back this.backlog.
    }

    command enqueue(count tasks, count priority) : count
    {
      this.backlog := this.backlog add tasks add priority.
      back this.backlog.
    }
  }

  deploy MissionQueue q(5).

  telemetry count r1 := route(3).
  telemetry count r2 := route(3, 4).
  transmit r1.
  transmit r2.
  transmit q.enqueue(2).
  transmit q.enqueue(2, 1).

  telemetry count backlog := 77.
  verify (1 == 1)
  {
    telemetry count backlog := 500.
    transmit backlog.
  }

  transmit backlog.
}
success
