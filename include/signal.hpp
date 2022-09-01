/**
 * @file signal.hpp
 * @author botamochi6277 (botamochi6277@icloud.com)
 * @brief 
 * @version 0.1
 * @date 2021-08-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

template <typename T>
T map(T value, T in_min, T in_max, T out_min, T out_max)
{
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
/**
 * @brief make Triangle Pulse
 * _
 * |            ^
 * h           / \
 * |    x     /   \
 * - ---|-----     -------
 *            |-w-|
 * 
 * @param x 
 * @param center center of the pulse (peak)
 * @param height hight of the pulse
 * @param width width of the pulse
 * @return wave value at x
 */
template <typename T>
T TrianglePulse(T x, T center, T height = 255, T width = 1000)
{
    T begin_point = center - 0.5 * width;
    T end_point = center + 0.5 * width;

    if (x <= begin_point)
    {
        return 0;
    }

    if ((begin_point < x) && (x <= center))
    {
        return map(x, begin_point, center, 0.0, height);
    }

    if ((center < x) && (x <= end_point))
    {
        return map(x, center, end_point, height, 0.0);
    }

    if (end_point < x)
    {
        return 0;
    }
    return 0;
}

template <typename T>
T SinePulse(T x, T center, T height = 255, T width = 1000)
{
    T begin_point = center - 0.5 * width;
    T end_point = center + 0.5 * width;

    if ((x <= begin_point) || (end_point < x))
    {
        return 0;
    }

    if ((begin_point < x) && (x <= end_point))
    {
        T xx = x - begin_point;
        T t = map(xx, T(0), width, T(-3141), T(3141));
        float rad = t * 0.001; //-pi -- pi
        return 0.5 * height * cos(rad) + 0.5 * height;
    }
    return 0;
}

template <typename T>
T HeartPulse(T x, T center, T height = 255, T width = 1000)
{
    T begin_point = center - 0.5 * width;
    T end_point = center + 0.5 * width;
    // 1/6 = 0.167
    // 1/12 = 0.083
    T y_p = SinePulse(x, begin_point + T(width * 0.083), T(height * 0.25), T(width * 0.167));
    T y_q = TrianglePulse(x, center - T(width * 0.1), -T(height * 0.10), T(width * 0.083));
    T y_r = TrianglePulse(x, center, height, T(width * 0.167));
    T y_s = TrianglePulse(x, center + T(width * 0.1), -T(height * 0.15), T(width * 0.083));
    T y_t = SinePulse(x, end_point - T(width * 0.083), T(height * 0.5), T(width * 0.167));
    return y_p + y_q + y_r + y_s + y_t;
}
