/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   amp_clean_jpg;
    const int            amp_clean_jpgSize = 164414;

    extern const char*   amp_lead_jpg;
    const int            amp_lead_jpgSize = 164197;

    extern const char*   amp_off_jpg;
    const int            amp_off_jpgSize = 163782;

    extern const char*   bluej_clean_p0088_json;
    const int            bluej_clean_p0088_jsonSize = 171901;

    extern const char*   bluej_fullD_p0153_json;
    const int            bluej_fullD_p0153_jsonSize = 173723;

    extern const char*   knob_silver_png;
    const int            knob_silver_pngSize = 325315;

    extern const char*   led_blue_off_png;
    const int            led_blue_off_pngSize = 1437;

    extern const char*   led_blue_on_png;
    const int            led_blue_on_pngSize = 3329;

    extern const char*   led_blue_on_off_png;
    const int            led_blue_on_off_pngSize = 4537;

    extern const char*   led_red_off_png;
    const int            led_red_off_pngSize = 1458;

    extern const char*   led_red_on_png;
    const int            led_red_on_pngSize = 2375;

    extern const char*   power_switch_down_png;
    const int            power_switch_down_pngSize = 4490;

    extern const char*   power_switch_up_png;
    const int            power_switch_up_pngSize = 4489;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 13;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
