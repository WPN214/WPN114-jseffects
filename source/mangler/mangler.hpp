/* adaptation of remaincalm.org Paranoia Mangler JS effect for Reaper
 * author: Daniel Arena (dan@remaincalm.org)
*/

#ifndef MANGLER_HPP
#define MANGLER_HPP

#include <wpn114audio/graph.hpp>
#include <cmath>

#define fOR(_lhs, _rhs) \
    static_cast<float>(static_cast<int>(_lhs) | static_cast<int>(_rhs))

#define fAND(_lhs, _rhs) \
    static_cast<float>(static_cast<int>(_lhs) & static_cast<int>(_rhs))

//-------------------------------------------------------------------------------------------------
class Mangler : public Node
//-------------------------------------------------------------------------------------------------
{
    Q_OBJECT

    WPN_DECLARE_DEFAULT_AUDIO_INPUT(audio_in, 2)
    WPN_DECLARE_AUDIO_INPUT(input_gain, 1)
    WPN_DECLARE_AUDIO_INPUT(dry, 1)
    WPN_DECLARE_AUDIO_INPUT(wet, 1)
    WPN_DECLARE_AUDIO_INPUT(resampler, 1)
    WPN_DECLARE_AUDIO_INPUT(bitcrusher, 1)
    WPN_DECLARE_AUDIO_INPUT(thermonuclear, 1)
    WPN_DECLARE_AUDIO_INPUT(bitdepth, 1)
    WPN_DECLARE_AUDIO_INPUT(gate, 1)
    WPN_DECLARE_AUDIO_INPUT(love, 1)
    WPN_DECLARE_AUDIO_INPUT(jive, 1)
    WPN_DECLARE_AUDIO_INPUT(attitude, 1)

    WPN_DECLARE_DEFAULT_AUDIO_OUTPUT(audio_out, 2)

    enum inputs { audio_in, input_gain, dry, wet, resampler, bitcrusher, thermonuclear,
                  bitdepth, gate, love, jive, attitude };

public:
    //-------------------------------------------------------------------------------------------------
    Mangler() { m_name = "ParanoiaMangler"; }

    //-------------------------------------------------------------------------------------------------
    virtual void
    initialize(Graph::properties const& properties) override
    //-------------------------------------------------------------------------------------------------
    {
        int8_t patterns[17][8] =
        {
            {   1,  1,  1,  1,  1,  1,  1,  1 },
            {   0,  1,  1,  1,  1,  1,  1,  1 },
            {  -1,  1,  1,  1,  1,  1,  1,  1 },
            {   1,  0,  1,  1,  1,  1,  1,  1 },
            {   1, -1,  1,  1,  1,  1,  1,  1 },
            {   1,  0,  0,  1,  1,  1,  1,  1 },
            {  -1, -1,  1,  1,  1,  1,  1,  1 },
            {   1,  1,  1,  1, -1,  1,  1,  1 },
            {   1,  1,  1,  1,  1,  0,  1,  1 },
            {   1, -1,  0, -1,  0,  1, -1,  0 },
            {   1, -1,  1, -1,  1,  1, -1, -1 },
            {   0,  0,  0,  1,  0,  0,  0,  0 },
            {   1,  0, -1,  0,  0,  0,  0,  0 },
            {   1, -1,  0,  0,  1,  1,  0,  0 },
            {  -1,  0, -1,  1,  1,  0,  0,  0 },
            {  -1, -1,  1,  1,  0,  0,  0,  0 },
            {   1,  1,  1,  1,  0,  0,  0,  0 }
        };

        for (uint8_t n = 0; n < 17; ++n)
             set_bit_pattern(n*8, patterns[n]);

        m_srate = properties.rate;
    }

    //-------------------------------------------------------------------------------------------------
    virtual void
    on_rate_changed(sample_t rate) override
    //-------------------------------------------------------------------------------------------------
    {
        m_srate = rate;
    }

    //-------------------------------------------------------------------------------------------------
    virtual void
    rwrite(pool& inputs, pool& outputs, vector_t nframes) override
    //-------------------------------------------------------------------------------------------------
    {
        auto audio_in       = inputs.audio[Mangler::audio_in];
        auto input_gain     = inputs.audio[Mangler::input_gain][0];
        auto dry            = inputs.audio[Mangler::dry][0];
        auto wet            = inputs.audio[Mangler::wet][0];
        auto resampler      = inputs.audio[Mangler::resampler][0];
        auto bitcrusher     = inputs.audio[Mangler::bitcrusher][0];
        auto thermonuclear  = inputs.audio[Mangler::thermonuclear][0];
        auto bitdepth       = inputs.audio[Mangler::bitdepth][0];
        auto gate           = inputs.audio[Mangler::gate][0];
        auto love           = inputs.audio[Mangler::love][0];
        auto jive           = inputs.audio[Mangler::jive][0];
        auto attitude       = inputs.audio[Mangler::attitude][0];

        auto srate          = m_srate;

        auto audio_out      = outputs.audio[0];

        auto thermonuclear_f = thermonuclear[0];
        auto gate_f = gate[0];
        auto bitdepth_f = bitdepth[0];
        auto resampler_f = resampler[0];
        auto dry_f = dry[0];
        auto wet_f = wet[0];
        auto gain_f = input_gain[0];


        // gate parameters -----------------------------------------------------
        float gate_amt          = gate_f/100.f;
        float gate_open_time    = (0.05f + (1.f-gate_amt)*0.3f) * srate;
        float fade_point        = gate_open_time*0.5f;
        float gate_threshold    = 0.15f + gate_amt * 0.25f;
        float gate_leakage      = (gate_amt > 0.5f) ? 0 : (1.f-gate_amt)*0.2f;

        int resol               = std::pow(2.f, bitdepth_f-1.f);
        float invresl           = 1.f/resol;
        float target_per_sample = (float) srate/resampler_f;

        float gain              = std::pow(2.f, gain_f/6.f);
        float dry_gain          = std::pow(2.f, dry_f/6.f);
        float wet_gain          = std::pow(2.f, wet_f/6.f);

        // LOOKUP -----------------------------------------------------------------------

        int left_bit_slider     = (int) thermonuclear_f | 0;
        float mix               = thermonuclear_f-(float)left_bit_slider;
        int right_bit_slider    = ( mix > 0 ) ? left_bit_slider + 1 : left_bit_slider;

        int bit_1 = left_bit_slider*16;
        int bit_2 = right_bit_slider*16;

        if (bitdepth_f < 8) {
            bit_1 += 8-bitdepth_f;
            bit_2 += 8-bitdepth_f;
        }
        else
        {
            for ( quint8 b = 0; b < (bitdepth_f-8); ++b )
            {
                bit_1--;
                bit_2--;
                lut[bit_1] = 1;
                lut[bit_2] = 1;
            }
        }

        if (bit_1 < 0) bit_1 = 0;
        if (bit_2 < 0) bit_2 = 0;

        float clear_mask_1  = 0, clear_mask_2   = 0;
        float xor_mask_1    = 0, xor_mask_2     = 0;

        for (uint32_t i = 0; i < bitdepth_f; ++i)
        {
            if (lut[bit_1+i] == 0)
                clear_mask_1 = fOR(clear_mask_1, pow(2, i));

            else if (lut[bit_1+i] == -1)
                xor_mask_1 = fOR(xor_mask_1, pow(2, i));

            if (lut[bit_2+i] == 0)
                clear_mask_2 = fOR(clear_mask_2, pow(2, i));

            else if(lut[bit_2+i] == -1)
                xor_mask_2 = fOR(xor_mask_2, pow(2, i));
        }

        float
        post_bit_gain = relgain[left_bit_slider]  * (1.f-mix) +
                        relgain[right_bit_slider] * mix;

        // RC filter params (hi/lo) -----------------------------------

        auto love_f = love[0];
        auto jive_f = jive[0];

        float
        LPF_c = std::pow(0.5f, 5.f-love_f/25.f ),
        LPF_r = std::pow(0.5f, jive_f/40.f-0.6f ),
        HPF_c = std::pow(0.5f, 5.f-love_f/32.f ),
        HPF_r = std::pow(0.5f, 3.f-jive_f/40.f );

        // precalc ----------------------------------------------------

        float
        LRC = LPF_r*LPF_c,
        HRC = HPF_r*HPF_c;

        // @SAMPLE ====================================================

        for(size_t f = 0; f < nframes; ++f)
        {
            float
            dry_0 = audio_in[0][f],
            dry_1 = audio_in[1][f],
            f0 = 0.f,
            f1 = 0.f;

            per_sample =  0.9995f * per_sample + 0.0005f * target_per_sample;

            // deliberately broken resampler
            // (BAD DIGITAL) -----------------------------------------
            sample_csr++;

            auto resampler_f = resampler[f];

            if (sample_csr < next_sample && resampler_f < 33150) {
                f0 = last_spl0;
                f1 = last_spl1;
            }
            else
            {
                // for resampler - this doesn't work properly but sounds cool
                next_sample += per_sample;
                if (resampler_f == 33150.f)
                    sample_csr = next_sample;

                f0 = dry_0*gain;
                f1 = dry_1*gain;
            }

            // skip gate for now -------------------------------------------
            // and shape ---------------------------------------------------
            f0 = ( 1.f+shaper_amt )*f0/(1.f+shaper_amt*abs(f0) );
            f1 = ( 1.f+shaper_amt )*f1/(1.f+shaper_amt*abs(f1) );

            // clamp ------------------------------------------------------
            f0 = qMax(qMin(f0, 0.95f), -0.95f);
            f1 = qMax(qMin(f1, 0.95f), -0.95f);

            auto bitcrusher_f = bitcrusher[f];

            // bitcrush --------------------------------------------------
            if (bitcrusher_f != 0.f)
            {
                // boost to positive range: 0->255
                // SOMETHING GOES WRONG HERE
                if (bitcrusher_f == 2.f) {
                    f0 = (int)((dcshift+f0) * resol ) | 0;
                    f1 = (int)((dcshift+f1) * resol ) | 0;
                }
                // boost to positive range, -resol to +resol-1
                else
                {
                    bitdepth_f = bitdepth[f];

                    f0 = (int)(f0*resol) | 0;
                    f1 = (int)(f1*resol) | 0;
                    // 2s complement
                    if (f0 < 0)
                        f0 = std::pow(2, bitdepth_f)+f0;

                    if (f1 < 0)
                        f1 = std::pow(2, bitdepth_f)+f1;
                }

                auto thermonuclear_f = thermonuclear[f];

                // mangle----------------------------------------------------
                if (thermonuclear_f > 0)
                {
                    float
                    f0A = fAND(f0, 1023.f-clear_mask_1);
                    f0A = fOR(fAND(f0A, 1023.f-xor_mask_1),
                              fAND(1023.f-f0A, xor_mask_1));

                    float
                    f1A = fAND(f1, 1023.f-clear_mask_1);
                    f1A = fOR(fAND( f1A, 1023.f-xor_mask_1),
                              fAND(1023.f-f1A, xor_mask_1));

                    float
                    f0B = fAND(f0, 1023.f-clear_mask_2);
                    f0B = fOR(fAND(f0B, 1023.f-xor_mask_2),
                              fAND(1023.f-f0B, xor_mask_2));

                    float
                    f1B = fAND(f1, 1023.f-clear_mask_2);
                    f1B = fOR(fAND(f1B, 1023.f-xor_mask_2),
                              fAND(1023.f-f1B, xor_mask_2));

                    f0 = f0A * (1.f-mix) + f0B * mix;
                    f1 = f1A * (1.f-mix) + f1B * mix;
                }

                if (bitcrusher_f == 2.f) {
                    f0 = (f0 *invresl - dcshift);
                    f1 = (f1 *invresl - dcshift);
                } else {
                    f0 = (f0 * invresl );
                    f1 = (f1 * invresl );
                    if (f0 > 1.f) f0 -= 2.f;
                    if (f1 > 1.f) f1 -= 2.f;
                }
            }

            // remember last_sample
            last_spl0 = f0;
            last_spl1 = f1;

            auto attitude_f = attitude[f];

            // LPF ===================================
            if (attitude_f == 1.f || attitude_f == 2.f)
            {
                v0L = (1.f-LRC) *v0L - LPF_c*(v1L - f0);
                v1L = (1.f-LRC) *v1L + LPF_c*v0L;
                f0 = v1L;

                v0R = (1.f-LRC) *v0R - LPF_c*(v1R - f1);
                v1R = (1.f-LRC) *v1R + LPF_c*v0R;
                f1 = v1R;
            }

            // HPF ===================================
            if ( attitude_f == 2.f || attitude_f == 3.f )
            {
                hv0L = (1.f-HRC) *hv0L - HPF_c*(hv1L-f0);
                hv1L = (1.f-HRC) *hv1L + HPF_c*hv0L;
                f0 -= hv1L;

                hv0R = (1.f-HRC) *hv0R - HPF_c*(hv1R-f1);
                hv1R = (1.f-HRC) *hv1R + HPF_c*hv0R;
                f1 -= hv1R;
            }

            // waveshape again, just because ------------------
            f0 *= wet_gain;
            f1 *= wet_gain;

            f0 = (1.f+shaper_amt_2)*f0/(1.f+shaper_amt_2*fabs(f0));
            f1 = (1.f+shaper_amt_2)*f1/(1.f+shaper_amt_2*fabs(f1));

            // dcfilter --------------------------------------
            otm1 = 0.99f*otm1+f0-itm1;
            itm1 = f0;
            f0 = otm1;

            otm2 = 0.99f*otm2+f1-itm2;
            itm2 = f1;
            f1 = otm2;

            // try and handle weird bit pattern supergain
            if (bitcrusher_f > 0) {
                f0 *= post_bit_gain;
                f1 *= post_bit_gain;
            }

            // mix ------------------------------------------
            audio_out[0][f] = f0+dry_0*dry_gain;
            audio_out[1][f] = f1+dry_1*dry_gain;
        }

    }

private:
    //-------------------------------------------------------------------------------------------------
    void
    set_bit_pattern(uint16_t index, int8_t* pattern)
    //-------------------------------------------------------------------------------------------------
    {
        int8_t* ptr = &(lut[index]);
        memcpy(ptr, pattern, 8);
    }

    //-------------------------------------------------------------------------------------------------
    int8_t
    lut[136];

    sample_t
    itm1 = 0, itm2 = 0, otm1 = 0, otm2 = 0;

    sample_t
    dcshift = 1;

    sample_t
    relgain[17] =  {
      1, 1, 1, 1,
      1, 1, 1, 0.4f,
      0.08f, 0.5f, 0.08f, 1.5f,
      3, 0.2f, 2, 1, 1
    };

    sample_t
    shaper_amt      = 0.857f,
    shaper_amt_2    = 0.9f;

    sample_t
    sample_csr  = 0,
    per_sample  = 0,
    last_spl0   = 0,
    last_spl1   = 0,
    next_sample = 0;

    sample_t
    v0L = 0.f,  v0R = 0.f,
    v1L = 0.f,  v1R = 0.f,
    hv0L = 0.f, hv0R = 0.f,
    hv1L = 0.f, hv1R = 0.f;

    sample_t
    m_srate = 0;
};

#endif // MANGLER_HPP
