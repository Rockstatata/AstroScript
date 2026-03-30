mission OperatorsShowcase launch
{
  telemetry count a := 12.
  telemetry count b := 5.
  telemetry count out.

  out := a add b.
  transmit out.
  out := a + b.
  transmit out.

  out := a minus b.
  transmit out.
  out := a - b.
  transmit out.

  out := a mul b.
  transmit out.
  out := a * b.
  transmit out.

  out := a divide b.
  transmit out.
  out := a / b.
  transmit out.

  out := a mod b.
  transmit out.

  out := a ** 2.
  transmit out.
}
success
