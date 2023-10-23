#include "iban/bban.h"

namespace iban
{
namespace countries
{

class BBan_handler_HU : public BBan_handler
{
    public:
    // Returns the coutry code
    std::string const& get_country() const override;

    // Validate the structural identity of the BBAN
    bool is_valid(std::string const& bban) const override;

    // Validate if the BBAN's length is valid
    bool is_valid_length(std::string const& bban) const override;

    // Validate the checksum(s) inside the BBAN
    bool is_valid_checksum(std::string const& bban) const override;

    // Validate the bankcode (if present in the BBAN)
    bool is_valid_bankcode(std::string const& bban) const override;

    // Validate the bban with an externally provided validator (e.g. bank specific check)
    //
    // It is implemented as a weak function, hence one can replace with own code (the
    // base simply returnes true) 
    bool is_valid_ext(std::string const& bban) const override;

    // Makes BBAN suitable for IBAN (e.g. if there are short BBAN formats, etc.)
    std::string preformat(std::string const& bban) const override;

    // Cut short the BBAN, if it has both long and short format;
    std::string trim(std::string const& bban) const override;

    // Formats BBAN according to the local presentation
    std::string format(std::string const& bban) const override;

    private:
    static const std::string m_country;
};

} // namespace countries
} // namespace iban