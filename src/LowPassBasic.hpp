
#ifndef LOWPASSBASIC_HPP_
#define LOWPASSBASIC_HPP_

class LowPassBasic
{
    private:
        float lastSample;

    public:
        LowPassBasic();
        float transform( float s );
};

inline LowPassBasic::LowPassBasic () :
    lastSample (0.0f)
{

}

inline float LowPassBasic::transform(float input) {
    float result = (lastSample + input) / 2.0f;
    lastSample = input;
    return result;
}

#endif