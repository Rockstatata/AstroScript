mission ComparisonsLogic launch
{
  telemetry count x := 6.
  telemetry count y := 3.

  verify (x > y) { transmit "GT". }
  verify (x < y) { transmit "LT". } otherwise { transmit "NOT_LT". }
  verify (x >= y) { transmit "GE". }
  verify (x <= y) { transmit "LE". } otherwise { transmit "NOT_LE". }
  verify (x == y) { transmit "EQ". } otherwise { transmit "NOT_EQ". }
  verify (x != y) { transmit "NEQ". }

  verify ((x > 0) AND (y > 0)) { transmit "AND". }
  verify ((x < 0) OR (y > 0)) { transmit "OR". }
  verify ((x > 0) XOR (y < 0)) { transmit "XOR". }
  verify (NOT (x < 0)) { transmit "NOT". }
}
success
