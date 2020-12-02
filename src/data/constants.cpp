#include "constants.h"

namespace constants {

// Github jgehrcke
const QString germanyGithubRepoUrl  = "https://github.com/jgehrcke/covid-19-germany-gae";
const QString germanyRepoDataUrl    = "https://raw.githubusercontent.com/jgehrcke/covid-19-germany-gae/master/";

const QString casesByState  = "cases-rki-by-state.csv";
const QString casesByAgs    = "cases-rki-by-ags.csv";
const QString deathsByState = "deaths-rki-by-state.csv";
const QString deathsByAgs   = "deaths-rki-by-ags.csv";
const QString ags           = "ags.json";

// Github JHU
const QString jhuGithubRepoUrl  = "https://github.com/CSSEGISandData/COVID-19";
const QString jhuRepoDataUrl    = "https://raw.githubusercontent.com/CSSEGISandData/COVID-19/master/csse_covid_19_data/csse_covid_19_time_series/";

const QString casesUS       = "time_series_covid19_confirmed_US.csv";
const QString casesGlobal   = "time_series_covid19_confirmed_global.csv";
const QString deathsUS      = "time_series_covid19_deaths_US.csv";
const QString deathsGlobal  = "time_series_covid19_deaths_global.csv";

// local data
const QString germanyStateJson = ":/de_bundeslaender.json";
const QString germanyShapeFile = "data/Kreisgrenzen_2018_mit_Einwohnerzahl-shp/KRS_18_ew.shp";

const QString usaStateJson = ":/us_states.json";
const QString usaShapeFile = "data/cb_2018_us_county_20m/cb_2018_us_county_20m.shp";

}
