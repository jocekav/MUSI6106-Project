/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2020 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 6 End-User License
   Agreement and JUCE Privacy Policy (both effective as of the 16th June 2020).

   End User License Agreement: www.juce.com/juce-6-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{
namespace dsp
{

/**
    Applies waveshaping to audio samples as single samples or AudioBlocks.

    @tags{DSP}
*/
template <typename FloatType>
struct TriodeWaveShaper
{
    //==============================================================================
    /** Called before processing starts. */
    void prepare (const ProcessSpec&) noexcept   {}

    //==============================================================================
    /** Returns the result of processing a single sample. */
    template <typename SampleType>
    SampleType JUCE_VECTOR_CALLTYPE processSample (SampleType inputSample) const noexcept
    {   
        return CommomCatode(inputSample);
    }

    /** Processes the input and output buffers supplied in the processing context. */
    template <typename ProcessContext>
    void process (const ProcessContext& context) const noexcept
    {
        auto&& inBlock = context.getInputBlock();
        auto&& outBlock = context.getOutputBlock();

        jassert(inBlock.getNumChannels() == outBlock.getNumChannels());
        jassert(inBlock.getNumSamples() == outBlock.getNumSamples());

        auto len = inBlock.getNumSamples();
        auto numChannels = inBlock.getNumChannels();


        if (context.isBypassed)
        {
            if (context.usesSeparateInputAndOutputBlocks())
                context.getOutputBlock().copyFrom (context.getInputBlock());
        }
        else
        {
            for (size_t chan = 0; chan < numChannels; ++chan)
            {
                auto* src = inBlock.getChannelPointer(chan);
                auto* dst = outBlock.getChannelPointer(chan);

                for (size_t i = 0; i < len; ++i)
                {
                    float E_1 = 0;
                    if (src[i] < V_gk_cutoff)
                        E_1 = (log(1 + exp(K_p * (mu_inverse + ((-1 * gain * src[i]) + V_ct) / (sqrt_K)))));
                    else
                        E_1 = (log(1 + exp(K_p * (mu_inverse + ((-1 * gain * V_gk_cutoff) + V_ct) / (sqrt_K)))));
                    dst[i] = (2.5 * 22.8f * (pow(static_cast<double>(E_1), static_cast<double>(E_x)) / K_g));
                }
            }
        }
    }

    void reset() noexcept {}

    //Triode Params;
    float mu = 100.0f;
    float mu_inverse = 1 / mu;
    float E_x = 1.4f;
    int K_g = 1060;
    int K_p = 600;
    int K_vb = 300;
    float V_ct = 0.5f;
    int V_ak = 280;
    float gain = 4;
    float V_gk_cutoff = 5.5f;
    float sqrt_K = static_cast<float> (sqrt(K_vb + (V_ak * V_ak)));

    template <typename SampleType>
    SampleType CommomCatode(SampleType V_gk)
    {
        float E_1 = 0;
        if (V_gk < V_gk_cutoff)
        {
            E_1 = (log(1 + exp(K_p * (mu_inverse + ((-1 * gain * V_gk) + V_ct) / (sqrt_K)))));
        }
        else
            E_1 = (log(1 + exp(K_p * (mu_inverse + ((-1 * gain * V_gk_cutoff) + V_ct) / (sqrt_K)))));
 
        return 2.5 * 22.8f * static_cast<SampleType> (pow(static_cast<double>(E_1), static_cast<double>(E_x)) / K_g);
    }
};

//==============================================================================
// 
//==============================================================================
template <typename Type>
class SingleTubeProcessor
{
public:
    SingleTubeProcessor() {};

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        auto& preFilterLow = singleTuberChain.template get<preFilterLowIndex>();
        preFilterLow.state = FilterCoefs::makeFirstOrderLowPass(spec.sampleRate, lowPassFilterFreq);
        auto& postFilterLow = singleTuberChain.template get<postFilterLowIndex>();
        postFilterLow.state = FilterCoefs::makeFirstOrderLowPass(spec.sampleRate, lowPassFilterFreq);
        auto& filterHigh = singleTuberChain.template get<filterHighIndex>();
        filterHigh.state = FilterCoefs::makeFirstOrderHighPass(spec.sampleRate, highOassFilterFreq);

        singleTuberChain.prepare(spec);
    };

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        singleTuberChain.process(context);
    };

    void reset() noexcept
    {
        singleTuberChain.reset();
    };

private:

    float lowPassFilterFreq = 10000;
    float highOassFilterFreq = 40;

    enum
    {
        preFilterLowIndex,
        tubeIndex,
        postFilterLowIndex,
        filterHighIndex
    };

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    juce::dsp::ProcessorChain<juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>, juce::dsp::TriodeWaveShaper<float>,
        juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>, juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>> singleTuberChain;


};


} // namespace dsp
} // namespace juce
