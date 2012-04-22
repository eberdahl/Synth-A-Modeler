/* =========================================================================================

   This is an auto-generated file, created by The Introjucer 3.0.0
   Do not edit anything in this file!

*/

namespace BinaryData
{
    extern const char*   mdl_file_header_txt;
    const int            mdl_file_header_txtSize = 875;

    extern const char*   Info_txt;
    const int            Info_txtSize = 407;

    extern const char*   prefs_audio_png;
    const int            prefs_audio_pngSize = 4248;

    extern const char*   prefs_about_png;
    const int            prefs_about_pngSize = 1819;

    extern const char*   prefs_misc_png;
    const int            prefs_misc_pngSize = 6162;

    extern const char*   icons_zip;
    const int            icons_zipSize = 10095;

    extern const char*   synthamodeler_icon_png;
    const int            synthamodeler_icon_pngSize = 25836;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();

    //==============================================================================
    // This class acts as an ImageProvider that will access the BinaryData images
    class ImageProvider  : public juce::ComponentBuilder::ImageProvider
    {
    public:
        ImageProvider() noexcept {}

        juce::Image getImageForIdentifier (const juce::var& imageIdentifier)
        {
            int dataSize = 0;
            const char* const data = getNamedResource (imageIdentifier.toString().toUTF8(), dataSize);

            if (data != nullptr)
                return juce::ImageCache::getFromMemory (data, dataSize);

            return juce::Image();
        }

        juce::var getIdentifierForImage (const juce::Image&)  { return juce::var(); }
    };
}
