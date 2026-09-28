#pragma once

#include "ofMain.h"
#include "ofJson.h"
#include <map>
#include <string>
#include <utility>

// Represents the current status of a currency request.
// Using an enum class (rather than a plain int) avoids accidental
// comparison with unrelated integers and keeps the states self-documenting.
enum class ApiState {
	Idle,
	Loading,
	Success,
	Error
};

// CurrencyAPI encapsulates all communication with freecurrencyapi.com.
// It hides networking and JSON parsing details behind a small public
// interface, and notifies the rest of the app via ofEvents rather than
// exposing its internal state directly (loose coupling).
class CurrencyAPI {
public:
	CurrencyAPI();
	~CurrencyAPI();

	// Loads the API key. Call once during setup().
	void setup(const std::string & apiKey);

	// Kicks off an asynchronous request to convert 'base' -> 'target'.
	// Results (or errors) arrive later via onRateReceived / onError.
	void requestRate(const std::string & baseCurrency, const std::string & targetCurrency);

	ApiState getState() const { return state; }
	std::string getErrorMessage() const { return errorMessage; }
	std::string getLastUpdated() const { return lastUpdated; }

	// Events other classes can subscribe to.
	ofEvent<double> onRateReceived;
	ofEvent<std::string> onError;

private:
	// Listener for the global OF URL response event.
	void urlResponse(ofHttpResponse & response);

	std::string apiKey;
	std::string requestedBase;
	std::string requestedTarget;

	ApiState state;
	std::string errorMessage;
	std::string lastUpdated;

	// Simple in-memory cache so we don't hammer the API with repeat
	// requests for a pair we already know. Key = "BASE_TARGET".
	// Value  = {rate, timeOfCache (seconds since app start)}.
	std::map<std::string, std::pair<double, float>> rateCache;
	const float cacheLifetimeSeconds = 300.0f; // 5 minutes
};
