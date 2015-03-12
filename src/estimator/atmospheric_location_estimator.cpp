#include "estimator/atmospheric_location_estimator.hpp"

#include "protocol/messages.hpp"

#include "unit_config.hpp"

AtmosphericLocationEstimator::AtmosphericLocationEstimator(Communicator& communicator)
  : locationMessageStream(communicator, 5) {
}

LocationEstimate AtmosphericLocationEstimator::update(const SensorMeasurements& meas) {
  makeEstimate(meas);
  updateStream();

  return loc;
}

LocationEstimate AtmosphericLocationEstimator::makeEstimate(const SensorMeasurements& meas) {
  if(meas.gps) {
    loc.lat = (*meas.gps).lat;
    loc.lon = (*meas.gps).lat;
  }

  if(meas.bar) {
    // TODO: Mix GPS and barometer readings to get an accuration altitude?
    // TODO: Pressure != altitude.
    loc.alt = (*meas.bar).pressure;
  }

  return loc;
}

void AtmosphericLocationEstimator::updateStream() {
  if(locationMessageStream.ready()) {
    protocol::message::location_message_t m {
      .lat = loc.lat,
      .lon = loc.lon,
      .alt = loc.alt
    };

    locationMessageStream.publish(m);
  }
}
