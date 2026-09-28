#include "CurrencyAPI.h"

CurrencyAPI::CurrencyAPI()
	: state(ApiState::Idle)
{
	// Subscribe to OF's global asynchronous URL response event once.
	ofAddListener(ofURLResponseEvent(), this, &CurrencyAPI::urlResponse);
}

CurrencyAPI::~CurrencyAPI() {
	ofRemoveListener(ofURLResponseEvent(), this, &CurrencyAPI::urlResponse);
}

void CurrencyAPI::setup(const std::string & key) {
	apiKey = key;
}

void CurrencyAPI::requestRate(const std::string & base, const std::string & target) {

	// Converting a currency to itself is always a 1:1 rate - no need
	// to call the API at all.
	if (base == target) {
		state = ApiState::Success;
		lastUpdated = ofGetTimestampString("%H:%M:%S");
		double rate = 1.0;
		ofNotifyEvent(onRateReceived, rate, this);
		return;
	}

	std::string cacheKey = base + "_" + target;

	// Check the cache first to avoid unnecessary network calls.
	auto it = rateCache.find(cacheKey);
	float now = ofGetElapsedTimef();
	if (it != rateCache.end() && (now - it->second.second) < cacheLifetimeSeconds) {
		state = ApiState::Success;
		lastUpdated = ofGetTimestampString("%H:%M:%S");
		double cachedRate = it->second.first;
		ofNotifyEvent(onRateReceived, cachedRate, this);
		return;
	}

	if (apiKey.empty()) {
		errorMessage = "No API key found. Add your freecurrencyapi.com key to bin/data/apikey.txt";
		state = ApiState::Error;
		ofNotifyEvent(onError, errorMessage, this);
		return;
	}

	requestedBase = base;
	requestedTarget = target;
	state = ApiState::Loading;

	std::string url = "https://api.freecurrencyapi.com/v1/latest?apikey=" + apiKey +
		"&base_currency=" + base + "&currencies=" + target;

	// Non-blocking request - the UI stays responsive while this is in flight.
	// 'cacheKey' is passed as the request name so the response can be matched
	// back to what was asked for.
	ofLoadURLAsync(url, cacheKey);
}

void CurrencyAPI::urlResponse(ofHttpResponse & response) {

	// Ignore responses that don't belong to the most recent request we
	// are waiting on (e.g. a slow, superseded request arriving late).
	std::string expectedName = requestedBase + "_" + requestedTarget;
	if (response.request.name != expectedName) {
		return;
	}

	if (response.status != 200) {
		errorMessage = "API request failed (HTTP " + ofToString(response.status) +
			"). Check your internet connection or API key.";
		state = ApiState::Error;
		ofNotifyEvent(onError, errorMessage, this);
		return;
	}

	try {
		ofJson json = ofJson::parse(response.data.getText());

		if (!json.contains("data")) {
			throw std::runtime_error("Unexpected response format from API.");
		}
		if (!json["data"].contains(requestedTarget)) {
			throw std::runtime_error("Currency '" + requestedTarget + "' was not returned by the API.");
		}

		double rate = json["data"][requestedTarget].get<double>();

		state = ApiState::Success;
		lastUpdated = ofGetTimestampString("%H:%M:%S");

		std::string cacheKey = requestedBase + "_" + requestedTarget;
		rateCache[cacheKey] = std::make_pair(rate, ofGetElapsedTimef());

		ofNotifyEvent(onRateReceived, rate, this);
	}
	catch (const std::exception & e) {
		errorMessage = std::string("Could not read API response: ") + e.what();
		state = ApiState::Error;
		ofNotifyEvent(onError, errorMessage, this);
	}
}
