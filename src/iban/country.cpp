/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */ 

#include "iban/country.h"
#include "iban/error.h"

#include <memory>

using std::function;
using std::map;
using std::optional;
using std::reference_wrapper;
using std::string;
using std::swap;
using std::transform;
using std::unique_ptr;

namespace iban
{
Country::operator std::string() const
{
    return name;
}

map<string, Country> Country_repository::m_countries = {
{"AF", {"AF", "AFG", "004", "Afghanistan", 1, 0, 0, 0}},
{"AX", {"AX", "ALA", "248", "Åland Islands", 0, 0, 0, 0}},
{"AL", {"AL", "ALB", "008", "Albania", 1, 0, 1, 0}},
{"DZ", {"DZ", "DZA", "012", "Algeria", 1, 0, 1, 0}},
{"AS", {"AS", "ASM", "016", "American Samoa", 0, 0, 0, 0}},
{"AD", {"AD", "AND", "020", "Andorra", 1, 0, 1, 1}},
{"AO", {"AO", "AGO", "024", "Angola", 1, 0, 1, 0}},
{"AI", {"AI", "AIA", "660", "Anguilla", 0, 0, 0, 0}},
{"AQ", {"AQ", "ATA", "010", "Antarctica", 0, 0, 0, 0}},
{"AG", {"AG", "ATG", "028", "Antigua and Barbuda", 1, 0, 0, 0}},
{"AR", {"AR", "ARG", "032", "Argentina", 1, 0, 0, 0}},
{"AM", {"AM", "ARM", "051", "Armenia", 1, 0, 0, 0}},
{"AW", {"AW", "ABW", "533", "Aruba", 0, 0, 0, 0}},
{"AU", {"AU", "AUS", "036", "Australia", 1, 0, 0, 0}},
{"AT", {"AT", "AUT", "040", "Austria", 1, 0, 1, 1}},
{"AZ", {"AZ", "AZE", "031", "Azerbaijan", 1, 0, 1, 0}},
{"BS", {"BS", "BHS", "044", "Bahamas", 1, 0, 0, 0}},
{"BH", {"BH", "BHR", "048", "Bahrain", 1, 0, 1, 0}},
{"BD", {"BD", "BGD", "050", "Bangladesh", 1, 0, 0, 0}},
{"BB", {"BB", "BRB", "052", "Barbados", 1, 0, 0, 0}},
{"BY", {"BY", "BLR", "112", "Belarus", 1, 0, 1, 0}},
{"BE", {"BE", "BEL", "056", "Belgium", 1, 0, 1, 1}},
{"BZ", {"BZ", "BLZ", "084", "Belize", 1, 0, 0, 0}},
{"BJ", {"BJ", "BEN", "204", "Benin", 1, 0, 1, 0}},
{"BM", {"BM", "BMU", "060", "Bermuda", 0, 0, 0, 0}},
{"BT", {"BT", "BTN", "064", "Bhutan", 1, 0, 0, 0}},
{"BO", {"BO", "BOL", "068", "Bolivia (Plurinational State of)", 1, 0, 0, 0}},
{"BQ", {"BQ", "BES", "535", "Bonaire, Sint Eustatius and Saba", 0, 0, 0, 0}},
{"BA", {"BA", "BIH", "070", "Bosnia and Herzegovina", 1, 0, 1, 0}},
{"BW", {"BW", "BWA", "072", "Botswana", 1, 0, 0, 0}},
{"BV", {"BV", "BVT", "074", "Bouvet Island", 0, 0, 0, 0}},
{"BR", {"BR", "BRA", "076", "Brazil", 1, 0, 1, 0}},
{"IO", {"IO", "IOT", "086", "British Indian Ocean Territory", 0, 0, 0, 0}},
{"BN", {"BN", "BRN", "096", "Brunei Darussalam", 1, 0, 0, 0}},
{"BG", {"BG", "BGR", "100", "Bulgaria", 1, 0, 1, 1}},
{"BF", {"BF", "BFA", "854", "Burkina Faso", 1, 0, 1, 0}},
{"BI", {"BI", "BDI", "108", "Burundi", 1, 0, 1, 0}},
{"CV", {"CV", "CPV", "132", "Cabo Verde", 1, 0, 1, 0}},
{"KH", {"KH", "KHM", "116", "Cambodia", 1, 0, 0, 0}},
{"CM", {"CM", "CMR", "120", "Cameroon", 1, 0, 1, 0}},
{"CA", {"CA", "CAN", "124", "Canada", 1, 0, 0, 0}},
{"KY", {"KY", "CYM", "136", "Cayman Islands", 0, 0, 0, 0}},
{"CF", {"CF", "CAF", "140", "Central African Republic", 1, 0, 1, 0}},
{"TD", {"TD", "TCD", "148", "Chad", 1, 0, 1, 0}},
{"CL", {"CL", "CHL", "152", "Chile", 1, 0, 0, 0}},
{"CN", {"CN", "CHN", "156", "China", 1, 0, 0, 0}},
{"CX", {"CX", "CXR", "162", "Christmas Island", 0, 0, 0, 0}},
{"CC", {"CC", "CCK", "166", "Cocos {Keeling} Islands", 0, 0, 0, 0}},
{"CO", {"CO", "COL", "170", "Colombia", 1, 0, 0, 0}},
{"KM", {"KM", "COM", "174", "Comoros", 1, 0, 1, 0}},
{"CG", {"CG", "COG", "178", "Congo", 1, 0, 1, 0}},
{"CD", {"CD", "COD", "180", "Congo, Democratic Republic of the", 1, 0, 0, 0}},
{"CK", {"CK", "COK", "184", "Cook Islands", 0, 0, 0, 0}},
{"CR", {"CR", "CRI", "188", "Costa Rica", 1, 0, 1, 0}},
{"CI", {"CI", "CIV", "384", "Côte d'Ivoire", 1, 0, 1, 0}},
{"HR", {"HR", "HRV", "191", "Croatia", 1, 0, 1, 1}},
{"CU", {"CU", "CUB", "192", "Cuba", 1, 0, 0, 0}},
{"CW", {"CW", "CUW", "531", "Curaçao", 0, 0, 0, 0}},
{"CY", {"CY", "CYP", "196", "Cyprus", 1, 0, 1, 1}},
{"CZ", {"CZ", "CZE", "203", "Czechia", 1, 0, 1, 1}},
{"DK", {"DK", "DNK", "208", "Denmark", 1, 0, 1, 1}},
{"DJ", {"DJ", "DJI", "262", "Djibouti", 1, 0, 1, 0}},
{"DM", {"DM", "DMA", "212", "Dominica", 1, 0, 0, 0}},
{"DO", {"DO", "DOM", "214", "Dominican Republic", 1, 0, 1, 0}},
{"EC", {"EC", "ECU", "218", "Ecuador", 1, 0, 0, 0}},
{"EG", {"EG", "EGY", "818", "Egypt", 1, 0, 1, 0}},
{"SV", {"SV", "SLV", "222", "El Salvador", 1, 0, 1, 0}},
{"GQ", {"GQ", "GNQ", "226", "Equatorial Guinea", 1, 0, 1, 0}},
{"ER", {"ER", "ERI", "232", "Eritrea", 1, 0, 0, 0}},
{"EE", {"EE", "EST", "233", "Estonia", 1, 0, 1, 1}},
{"SZ", {"SZ", "SWZ", "748", "Eswatini", 1, 0, 0, 0}},
{"ET", {"ET", "ETH", "231", "Ethiopia", 1, 0, 0, 0}},
{"FK", {"FK", "FLK", "238", "Falkland Islands {Malvinas}", 0, 0, 0, 0}},
{"FO", {"FO", "FRO", "234", "Faroe Islands", 0, 0, 1, 0}},
{"FJ", {"FJ", "FJI", "242", "Fiji", 1, 0, 0, 0}},
{"FI", {"FI", "FIN", "246", "Finland", 1, 0, 1, 1}},
{"FR", {"FR", "FRA", "250", "France", 1, 0, 1, 1}},
{"GF", {"GF", "GUF", "254", "French Guiana", 0, 0, 0, 0}},
{"PF", {"PF", "PYF", "258", "French Polynesia", 0, 0, 0, 0}},
{"TF", {"TF", "ATF", "260", "French Southern Territories", 0, 0, 0, 0}},
{"GA", {"GA", "GAB", "266", "Gabon", 1, 0, 1, 0}},
{"GM", {"GM", "GMB", "270", "Gambia", 1, 0, 0, 0}},
{"GE", {"GE", "GEO", "268", "Georgia", 1, 0, 1, 0}},
{"DE", {"DE", "DEU", "276", "Germany", 1, 0, 1, 1}},
{"GH", {"GH", "GHA", "288", "Ghana", 1, 0, 0, 0}},
{"GI", {"GI", "GIB", "292", "Gibraltar", 0, 0, 1, 0}},
{"GR", {"GR", "GRC", "300", "Greece", 1, 0, 1, 1}},
{"GL", {"GL", "GRL", "304", "Greenland", 0, 0, 1, 0}},
{"GD", {"GD", "GRD", "308", "Grenada", 1, 0, 0, 0}},
{"GP", {"GP", "GLP", "312", "Guadeloupe", 0, 0, 0, 0}},
{"GU", {"GU", "GUM", "316", "Guam", 0, 0, 0, 0}},
{"GT", {"GT", "GTM", "320", "Guatemala", 1, 0, 1, 0}},
{"GG", {"GG", "GGY", "831", "Guernsey", 0, 0, 0, 0}},
{"GN", {"GN", "GIN", "324", "Guinea", 1, 0, 0, 0}},
{"GW", {"GW", "GNB", "624", "Guinea-Bissau", 1, 0, 1, 0}},
{"GY", {"GY", "GUY", "328", "Guyana", 1, 0, 0, 0}},
{"HT", {"HT", "HTI", "332", "Haiti", 1, 0, 0, 0}},
{"HM", {"HM", "HMD", "334", "Heard Island and McDonald Islands", 0, 0, 0, 0}},
{"VA", {"VA", "VAT", "336", "Holy See (Vatican City)", 1, 0, 1, 1}},
{"HN", {"HN", "HND", "340", "Honduras", 1, 0, 1, 0}},
{"HK", {"HK", "HKG", "344", "Hong Kong", 0, 0, 0, 0}},
{"HU", {"HU", "HUN", "348", "Hungary", 1, 0, 1, 1}},
{"IS", {"IS", "ISL", "352", "Iceland", 1, 0, 1, 1}},
{"IN", {"IN", "IND", "356", "India", 1, 0, 0, 0}},
{"ID", {"ID", "IDN", "360", "Indonesia", 1, 0, 0, 0}},
{"IR", {"IR", "IRN", "364", "Iran (Islamic Republic of)", 1, 0, 1, 0}},
{"IQ", {"IQ", "IRQ", "368", "Iraq", 1, 0, 1, 0}},
{"IE", {"IE", "IRL", "372", "Ireland", 1, 0, 1, 1}},
{"IM", {"IM", "IMN", "833", "Isle of Man", 0, 0, 0, 0}},
{"IL", {"IL", "ISR", "376", "Israel", 1, 0, 1, 0}},
{"IT", {"IT", "ITA", "380", "Italy", 1, 0, 1, 1}},
{"JM", {"JM", "JAM", "388", "Jamaica", 1, 0, 0, 0}},
{"JP", {"JP", "JPN", "392", "Japan", 1, 0, 0, 0}},
{"JE", {"JE", "JEY", "832", "Jersey", 0, 0, 0, 0}},
{"JO", {"JO", "JOR", "400", "Jordan", 1, 0, 1, 0}},
{"KZ", {"KZ", "KAZ", "398", "Kazakhstan", 1, 0, 1, 0}},
{"KE", {"KE", "KEN", "404", "Kenya", 1, 0, 0, 0}},
{"KI", {"KI", "KIR", "296", "Kiribati", 1, 0, 0, 0}},
{"KP", {"KP", "PRK", "408", "Korea (Democratic People's Republic of)", 1, 0, 0, 0}},
{"KR", {"KR", "KOR", "410", "Korea, Republic of", 1, 0, 0, 0}},
{"KW", {"KW", "KWT", "414", "Kuwait", 1, 0, 1, 0}},
{"KG", {"KG", "KGZ", "417", "Kyrgyzstan", 1, 0, 0, 0}},
{"LA", {"LA", "LAO", "418", "Lao People's Democratic Republic", 1, 0, 0, 0}},
{"LV", {"LV", "LVA", "428", "Latvia", 1, 0, 1, 1}},
{"LB", {"LB", "LBN", "422", "Lebanon", 1, 0, 1, 0}},
{"LS", {"LS", "LSO", "426", "Lesotho", 1, 0, 0, 0}},
{"LR", {"LR", "LBR", "430", "Liberia", 1, 0, 0, 0}},
{"LY", {"LY", "LBY", "434", "Libya", 1, 0, 1, 0}},
{"LI", {"LI", "LIE", "438", "Liechtenstein", 1, 0, 1, 1}},
{"LT", {"LT", "LTU", "440", "Lithuania", 1, 0, 1, 1}},
{"LU", {"LU", "LUX", "442", "Luxembourg", 1, 0, 1, 1}},
{"MO", {"MO", "MAC", "446", "Macao", 0, 0, 0, 0}},
{"MG", {"MG", "MDG", "450", "Madagascar", 1, 0, 1, 0}},
{"MW", {"MW", "MWI", "454", "Malawi", 1, 0, 0, 0}},
{"MY", {"MY", "MYS", "458", "Malaysia", 1, 0, 0, 0}},
{"MV", {"MV", "MDV", "462", "Maldives", 1, 0, 0, 0}},
{"ML", {"ML", "MLI", "466", "Mali", 1, 0, 1, 0}},
{"MT", {"MT", "MLT", "470", "Malta", 1, 0, 1, 1}},
{"MH", {"MH", "MHL", "584", "Marshall Islands", 1, 0, 0, 0}},
{"MQ", {"MQ", "MTQ", "474", "Martinique", 0, 0, 0, 0}},
{"MR", {"MR", "MRT", "478", "Mauritania", 1, 0, 1, 0}},
{"MU", {"MU", "MUS", "480", "Mauritius", 1, 0, 1, 0}},
{"YT", {"YT", "MYT", "175", "Mayotte", 0, 0, 0, 0}},
{"MX", {"MX", "MEX", "484", "Mexico", 1, 0, 0, 0}},
{"FM", {"FM", "FSM", "583", "Micronesia {Federated States of}", 1, 0, 0, 0}},
{"MD", {"MD", "MDA", "498", "Moldova, Republic of", 1, 0, 1, 0}},
{"MC", {"MC", "MCO", "492", "Monaco", 1, 0, 1, 1}},
{"MN", {"MN", "MNG", "496", "Mongolia", 1, 0, 0, 0}},
{"ME", {"ME", "MNE", "499", "Montenegro", 1, 0, 1, 0}},
{"MS", {"MS", "MSR", "500", "Montserrat", 0, 0, 0, 0}},
{"MA", {"MA", "MAR", "504", "Morocco", 1, 0, 1, 0}},
{"MZ", {"MZ", "MOZ", "508", "Mozambique", 1, 0, 1, 0}},
{"MM", {"MM", "MMR", "104", "Myanmar", 1, 0, 0, 0}},
{"NA", {"NA", "NAM", "516", "Namibia", 1, 0, 0, 0}},
{"NR", {"NR", "NRU", "520", "Nauru", 1, 0, 0, 0}},
{"NP", {"NP", "NPL", "524", "Nepal", 1, 0, 0, 0}},
{"NL", {"NL", "NLD", "528", "Netherlands", 1, 0, 1, 1}},
{"NC", {"NC", "NCL", "540", "New Caledonia", 0, 0, 0, 0}},
{"NZ", {"NZ", "NZL", "554", "New Zealand", 1, 0, 0, 0}},
{"NI", {"NI", "NIC", "558", "Nicaragua", 1, 0, 1, 0}},
{"NE", {"NE", "NER", "562", "Niger", 1, 0, 1, 0}},
{"NG", {"NG", "NGA", "566", "Nigeria", 1, 0, 0, 0}},
{"NU", {"NU", "NIU", "570", "Niue", 0, 0, 0, 0}},
{"NF", {"NF", "NFK", "574", "Norfolk Island", 0, 0, 0, 0}},
{"MK", {"MK", "MKD", "807", "North Macedonia", 1, 0, 1, 0}},
{"MP", {"MP", "MNP", "580", "Northern Mariana Islands", 0, 0, 0, 0}},
{"NO", {"NO", "NOR", "578", "Norway", 1, 0, 1, 1}},
{"OM", {"OM", "OMN", "512", "Oman", 1, 0, 0, 0}},
{"PK", {"PK", "PAK", "586", "Pakistan", 1, 0, 1, 0}},
{"PW", {"PW", "PLW", "585", "Palau", 1, 0, 0, 0}},
{"PS", {"PS", "PSE", "275", "Palestine, State of", 0, 0, 1, 0}},
{"PA", {"PA", "PAN", "591", "Panama", 1, 0, 0, 0}},
{"PG", {"PG", "PNG", "598", "Papua New Guinea", 1, 0, 0, 0}},
{"PY", {"PY", "PRY", "600", "Paraguay", 1, 0, 0, 0}},
{"PE", {"PE", "PER", "604", "Peru", 1, 0, 0, 0}},
{"PH", {"PH", "PHL", "608", "Philippines", 1, 0, 0, 0}},
{"PN", {"PN", "PCN", "612", "Pitcairn", 0, 0, 0, 0}},
{"PL", {"PL", "POL", "616", "Poland", 1, 0, 1, 1}},
{"PT", {"PT", "PRT", "620", "Portugal", 1, 0, 1, 1}},
{"PR", {"PR", "PRI", "630", "Puerto Rico", 0, 0, 0, 0}},
{"QA", {"QA", "QAT", "634", "Qatar", 1, 0, 1, 0}},
{"RE", {"RE", "REU", "638", "Réunion", 0, 0, 0, 0}},
{"RO", {"RO", "ROU", "642", "Romania", 1, 0, 1, 1}},
{"RU", {"RU", "RUS", "643", "Russian Federation", 1, 0, 0, 0}},
{"RW", {"RW", "RWA", "646", "Rwanda", 1, 0, 0, 0}},
{"BL", {"BL", "BLM", "652", "Saint Barthélemy", 0, 0, 0, 0}},
{"SH", {"SH", "SHN", "654", "Saint Helena, Ascension and Tristan da Cunha", 0, 0, 0, 0}},
{"KN", {"KN", "KNA", "659", "Saint Kitts and Nevis", 1, 0, 0, 0}},
{"LC", {"LC", "LCA", "662", "Saint Lucia", 1, 0, 1, 0}},
{"MF", {"MF", "MAF", "663", "Saint Martin {French part}", 0, 0, 0, 0}},
{"PM", {"PM", "SPM", "666", "Saint Pierre and Miquelon", 0, 0, 0, 0}},
{"VC", {"VC", "VCT", "670", "Saint Vincent and the Grenadines", 1, 0, 0, 0}},
{"WS", {"WS", "WSM", "882", "Samoa", 1, 0, 0, 0}},
{"SM", {"SM", "SMR", "674", "San Marino", 1, 0, 1, 1}},
{"ST", {"ST", "STP", "678", "Sao Tome and Principe", 1, 0, 1, 0}},
{"SA", {"SA", "SAU", "682", "Saudi Arabia", 1, 0, 1, 0}},
{"SN", {"SN", "SEN", "686", "Senegal", 1, 0, 1, 0}},
{"RS", {"RS", "SRB", "688", "Serbia", 1, 0, 1, 0}},
{"SC", {"SC", "SYC", "690", "Seychelles", 1, 0, 1, 0}},
{"SL", {"SL", "SLE", "694", "Sierra Leone", 1, 0, 0, 0}},
{"SG", {"SG", "SGP", "702", "Singapore", 1, 0, 0, 0}},
{"SX", {"SX", "SXM", "534", "Sint Maarten {Dutch part}", 0, 0, 0, 0}},
{"SK", {"SK", "SVK", "703", "Slovakia", 1, 0, 1, 1}},
{"SI", {"SI", "SVN", "705", "Slovenia", 1, 0, 1, 1}},
{"SB", {"SB", "SLB", "090", "Solomon Islands", 1, 0, 0, 0}},
{"SO", {"SO", "SOM", "706", "Somalia", 1, 0, 0, 0}},
{"ZA", {"ZA", "ZAF", "710", "South Africa", 1, 0, 0, 0}},
{"GS", {"GS", "SGS", "239", "South Georgia and the South Sandwich Islands", 0, 0, 0, 0}},
{"SS", {"SS", "SSD", "728", "South Sudan", 1, 0, 0, 0}},
{"ES", {"ES", "ESP", "724", "Spain", 1, 0, 1, 1}},
{"LK", {"LK", "LKA", "144", "Sri Lanka", 1, 0, 0, 0}},
{"SD", {"SD", "SDN", "729", "Sudan", 1, 0, 1, 0}},
{"SR", {"SR", "SUR", "740", "Suriname", 1, 0, 0, 0}},
{"SJ", {"SJ", "SJM", "744", "Svalbard and Jan Mayen", 0, 0, 0, 0}},
{"SE", {"SE", "SWE", "752", "Sweden", 1, 0, 1, 1}},
{"CH", {"CH", "CHE", "756", "Switzerland", 1, 0, 1, 1}},
{"SY", {"SY", "SYR", "760", "Syrian Arab Republic", 1, 0, 0, 0}},
{"TW", {"TW", "TWN", "158", "Taiwan, Province of China", 0, 0, 0, 0}},
{"TJ", {"TJ", "TJK", "762", "Tajikistan", 1, 0, 0, 0}},
{"TZ", {"TZ", "TZA", "834", "Tanzania, United Republic of", 1, 0, 0, 0}},
{"TH", {"TH", "THA", "764", "Thailand", 1, 0, 1, 0}},
{"TL", {"TL", "TLS", "626", "Timor-Leste", 1, 0, 1, 0}},
{"TG", {"TG", "TGO", "768", "Togo", 1, 0, 1, 0}},
{"TK", {"TK", "TKL", "772", "Tokelau", 0, 0, 0, 0}},
{"TO", {"TO", "TON", "776", "Tonga", 1, 0, 0, 0}},
{"TT", {"TT", "TTO", "780", "Trinidad and Tobago", 1, 0, 0, 0}},
{"TN", {"TN", "TUN", "788", "Tunisia", 1, 0, 1, 0}},
{"TR", {"TR", "TUR", "792", "Turkey", 1, 0, 1, 0}},
{"TM", {"TM", "TKM", "795", "Turkmenistan", 1, 0, 0, 0}},
{"TC", {"TC", "TCA", "796", "Turks and Caicos Islands", 0, 0, 0, 0}},
{"TV", {"TV", "TUV", "798", "Tuvalu", 1, 0, 0, 0}},
{"UG", {"UG", "UGA", "800", "Uganda", 1, 0, 0, 0}},
{"UA", {"UA", "UKR", "804", "Ukraine", 1, 0, 1, 0}},
{"AE", {"AE", "ARE", "784", "United Arab Emirates", 1, 0, 1, 0}},
{"GB", {"GB", "GBR", "826", "United Kingdom of Great Britain and Northern Ireland", 1, 0, 1, 1}},
{"US", {"US", "USA", "840", "United States of America", 1, 0, 0, 0}},
{"UM", {"UM", "UMI", "581", "United States Minor Outlying Islands", 0, 0, 0, 0}},
{"UY", {"UY", "URY", "858", "Uruguay", 1, 0, 0, 0}},
{"UZ", {"UZ", "UZB", "860", "Uzbekistan", 1, 0, 0, 0}},
{"VU", {"VU", "VUT", "548", "Vanuatu", 1, 0, 0, 0}},
{"VE", {"VE", "VEN", "862", "Venezuela {Bolivarian Republic of}", 1, 0, 0, 0}},
{"VN", {"VN", "VNM", "704", "Viet Nam", 1, 0, 0, 0}},
{"VG", {"VG", "VGB", "092", "Virgin Islands {British}", 0, 0, 1, 0}},
{"VI", {"VI", "VIR", "850", "Virgin Islands {U.S.}", 0, 0, 0, 0}},
{"WF", {"WF", "WLF", "876", "Wallis and Futuna", 0, 0, 0, 0}},
{"EH", {"EH", "ESH", "732", "Western Sahara", 0, 0, 0, 0}},
{"YE", {"YE", "YEM", "887", "Yemen", 1, 0, 0, 0}},
{"ZM", {"ZM", "ZMB", "894", "Zambia", 1, 0, 0, 0}},
{"ZW", {"ZW", "ZWE", "716", "Zimbabwe", 1, 0, 0, 0}},
{"XK", {"XK", "...", "...", "Kosovo", 1, 1, 1, 0}}};

Country_repository::Country_repository()
{
}

Country_repository* Country_repository::get_instance()
{
    static unique_ptr<Country_repository> m_instance(new Country_repository);
    return m_instance.get();
}

optional<reference_wrapper<Country const>> Country_repository::get_by_alpha2(string const& code) const
{
    string upper_code;
    transform(code.begin(), code.end(), std::back_inserter(upper_code), ::toupper);

    auto it = m_countries.find(upper_code);
    if (it == m_countries.end())
    {
        return {};
    }

    return it->second;
}

bool Country_repository::is_country(string const& code) const
{
    string upper_code;
    transform(code.begin(), code.end(), std::back_inserter(upper_code), ::toupper);

    return (m_countries.find(upper_code) != m_countries.end());
}

bool Country_repository::is_iban_country(string const& code) const
{
    string upper_code;
    transform(code.begin(), code.end(), std::back_inserter(upper_code), ::toupper);

    auto it = m_countries.find(upper_code);
    return (it != m_countries.end() && it->second.use_iban == true);
}

bool Country_repository::is_sepa_country(string const& code) const
{
    string upper_code;
    transform(code.begin(), code.end(), std::back_inserter(upper_code), ::toupper);

    auto it = m_countries.find(upper_code);
    return (it != m_countries.end() && it->second.is_sepa == true);
}

void Country_repository::load(function<void(map<string, Country> const&)> loader)
{
    map<string, Country> countries;
    loader(countries);

    swap(m_countries, countries);
}

} // namespace iban
