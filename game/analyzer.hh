#pragma once

#include "audio/audiosinkbase.hh"
#include "ringbuffer.hh"
#include "tone.hh"

#include <algorithm>
#include <cmath>
#include <complex>
#include <list>
#include <string>
#include <vector>

static const unsigned FFT_P = 10;
static const std::size_t FFT_N = 1 << FFT_P;

/// analyzer class
 /** class to analyze input audio and transform it into useable data
 */
class Analyzer : public audio::AudioSinkBase {
public:
	Analyzer(double rate, std::string id, std::size_t step = 200);
	Analyzer(const Analyzer&) = delete;
	const Analyzer& operator=(const Analyzer&) = delete;
	/// fast fourier transform vector
	typedef std::vector<std::complex<float> > fft_t;
	/// list of tones
	typedef std::list<Tone> tones_t;
	/** Add input data to buffer. This is thread-safe (against other functions). **/
	template <typename InIt> void input(InIt begin, InIt end) {
		m_buf.insert(begin, end);
		m_passthrough.insert(begin, end);
	}
	/** Call this to process all data input so far. **/
	void process();
	/** Get the raw FFT. **/
	fft_t const& getFFT() const { return m_fft; }
	/** Get the peak level in dB (negative value, 0.0 = clipping). **/
	double getPeak() const { return 10.0 * log10(m_peak); }
	/** Get a list of all tones detected. **/
	tones_t const& getTones() const { return m_tones; }
	/** Find a tone within the singing range; prefers strong tones around 200-400 Hz. **/
	Tone const* findTone(double minfreq = 65.0, double maxfreq = 1000.0) const;
	/** Give data away for mic pass-through */
	void output(float* begin, float* end, double rate);
	/** Returns the id (color name) of the mic */
	std::string const& getId() const { return m_id; }

	void write(std::vector<float>& in) override;

private:
	bool calcFFT();
	void calcTones();
	void mergeWithOld(tones_t& tones) const;

private:
	const std::size_t m_step;
	RingBuffer<2 * FFT_N> m_buf;  // Twice the FFT size should give enough room for sliding window and for engine delays
	RingBuffer<4096> m_passthrough;
	double m_resampleFactor;
	double m_resamplePos;
	double m_rate;
	std::string m_id;
	std::vector<float> m_window;
	fft_t m_fft;
	std::vector<float> m_fftLastPhase;
	double m_peak;
	tones_t m_tones;
	mutable double m_oldfreq;
};

