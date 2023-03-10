#include <iostream>
#include <fstream>
#include <random>
#include <cstdio>
#include <windows.h>


/* Structures */

struct Resources {
    int land = 0;
    int population = 0;
    int wheat = 0;
};

struct GameState {
    int round = 1;
    Resources current;
    Resources income;
    Resources waste;
    bool was_plague = false;
    int wheat_from_acre = 0;
    int land_price = 20;
};

struct Edict {
    int land_buy;
    int land_sell;
    int wheat_eat;
    int land_sow;
};

struct RoundParams {
    int wheat_from_acre;
    double rat_coefficient;
    bool plague;
};

struct Stats {
    double deaths_percentage = 0;
    double land_per_man = 0;
};

struct GameSave {
    GameState state;
    Stats stats;
};




/* Game rules & constants */

constexpr Resources START_RESOURCES = {
    1000, //land
    100, //population
    2800, //wheat
};

constexpr int ROUNDS_COUNT = 10;
constexpr int WHEAT_DEMAND_PER_MAN = 20;
constexpr int LAND_SOWN_BY_ONE_MAN = 10;
constexpr float WHEAT_FOR_SEEDS_PER_ACRE = 0.5;

constexpr int MIN_LAND_PRICE = 17;
constexpr int MAX_LAND_PRICE = 26;
constexpr int MIN_WHEAT_FROM_ACRE = 1;
constexpr int MAX_WHEAT_FROM_ACRE = 6;
constexpr double MAX_RATS_–°OEFFICIENT = 0.07;
constexpr double PLAGUE_POSSIBILITY = 15;

int newcomers_count( int died, int wheat, int wheat_from_acre )
{
    int newcomers = trunc( died / 2 + ( 5 - wheat_from_acre ) * wheat / 600 ) + 1;
    newcomers = newcomers < 0 ? 0 : ( newcomers > 50 ? 50 : newcomers );
    return newcomers;
};

constexpr Stats STATS_LEVEL_BAD = { 33, 7 };
constexpr Stats STATS_LEVEL_OKAY = { 10, 9 };
constexpr Stats STATS_LEVEL_GOOD = { 3, 10 };

constexpr double CRITICAL_DEATH_PERCENTAGE = 0.45;



/* Utils for output */

const char* numbered_word( const char* const* words, int k )
{
    switch ( k % 10 )
    {
    case 1:
        return ( k % 100 / 10 == 1 ) ? words[0] : words[1];
    case 2:
    case 3:
    case 4:
        return ( k % 100 / 10 == 1 ) ? words[0] : words[2];
    default:
        return words[0];
    }
}

const char* const words_man[] = { " —á–Ķ–Ľ–ĺ–≤–Ķ–ļ", " —á–Ķ–Ľ–ĺ–≤–Ķ–ļ", " —á–Ķ–Ľ–ĺ–≤–Ķ–ļa" };
const char* const words_acre[] = { " –į–ļ—Ä–ĺ–≤", " –į–ļ—Ä", " –į–ļ—Ä–į" };
const char* const words_bushel[] = { " –Ī—É—ą–Ķ–Ľ–Ķ–Ļ", " –Ī—É—ą–Ķ–Ľ—Ć", " –Ī—É—ą–Ķ–Ľ—Ź" };

const char* man( int k ) { return numbered_word( words_man, k ); }
const char* acre( int k ) { return numbered_word( words_acre, k ); }
const char* bushel( int k ) { return numbered_word( words_bushel, k ); }


/* Output */

void inform( const GameState& s )
{
    std::cout << "–ú–ĺ–Ļ –Ņ–ĺ–≤–Ķ–Ľ–ł—ā–Ķ–Ľ—Ć, —Ā–ĺ–ł–∑–≤–ĺ–Ľ—Ć –Ņ–ĺ–≤–Ķ–ī–į—ā—Ć —ā–Ķ–Ī–Ķ \n";
    std::cout << "–≤ –≥–ĺ–ī—É " << s.round << " —ā–≤–ĺ–Ķ–≥–ĺ –≤—č—Ā–ĺ—á–į–Ļ—ą–Ķ–≥–ĺ –Ņ—Ä–į–≤–Ľ–Ķ–Ĺ–ł—Ź\n";

    if ( s.waste.population > 0 )
    {
        std::cout << s.waste.population << man( s.waste.population ) << " —É–ľ–Ķ—Ä–Ľ–ł —Ā –≥–ĺ–Ľ–ĺ–ī—É";
        if ( s.income.population > 0 )
            std::cout << ", –ł ";
        else
            std::cout << ".\n";
    }
    if ( s.income.population > 0 )
        std::cout << s.income.population << man( s.income.population ) << " –Ņ—Ä–ł–Ī—č–Ľ–ł –≤ –Ĺ–į—ą –≤–Ķ–Ľ–ł–ļ–ł–Ļ –≥–ĺ—Ä–ĺ–ī.\n";

    if ( s.was_plague )
        std::cout << "–ß—É–ľ–į —É–Ĺ–ł—á—ā–ĺ–∂–ł–Ľ–į –Ņ–ĺ–Ľ–ĺ–≤–ł–Ĺ—É –Ĺ–į—Ā–Ķ–Ľ–Ķ–Ĺ–ł—Ź.\n";

    std::cout << "–Ě–į—Ā–Ķ–Ľ–Ķ–Ĺ–ł–Ķ –≥–ĺ—Ä–ĺ–ī–į —Ā–ĺ—Ā—ā–į–≤–Ľ—Ź–Ķ—ā " << s.current.population << man( s.current.population ) << ".\n";

    if ( s.income.wheat > 0 )
    {
        std::cout << "–ú—č —Ā–ĺ–Ī—Ä–į–Ľ–ł " << s.income.wheat << bushel( s.income.wheat ) << " –Ņ—ą–Ķ–Ĺ–ł—Ü—č, ";
        std::cout << s.wheat_from_acre << bushel( s.wheat_from_acre ) << " —Ā –į–ļ—Ä–į.";
        std::cout << "\n";
    }
    if ( s.waste.wheat > 0 )
    {
        std::cout << "–ö—Ä—č—Ā—č –ł—Ā—ā—Ä–Ķ–Ī–ł–Ľ–ł " << s.waste.wheat << bushel( s.waste.wheat ) << " –Ņ—ą–Ķ–Ĺ–ł—Ü—č,";
        std::cout << " –ĺ—Ā—ā–į–≤–ł–≤ " << s.current.wheat << bushel( s.current.wheat ) << " –≤ –į–ľ–Ī–į—Ä–į—Ö.\n";
    }
    else
        std::cout << "–í –į–ľ–Ī–į—Ä–į—Ö –ĺ—Ā—ā–į–Ľ–ĺ—Ā—Ć " << s.current.wheat << bushel( s.current.wheat ) << " –Ņ—ą–Ķ–Ĺ–ł—Ü—č.\n";

    std::cout << "–ď–ĺ—Ä–ĺ–ī —Ā–Ķ–Ļ—á–į—Ā –∑–į–Ĺ–ł–ľ–į–Ķ—ā " << s.current.land << acre( s.current.land ) << ".\n";

    std::cout << "1 –į–ļ—Ä –∑–Ķ–ľ–Ľ–ł —Ā—ā–ĺ–ł—ā —Ā–Ķ–Ļ—á–į—Ā " << s.land_price << bushel( s.land_price ) << ".\n\n";
}


void finish_game( const Stats& stats )
{
    std::cout << "–°—ā–į—ā–ł—Ā—ā–ł–ļ–į –ł–≥—Ä—č:\n";
    std::cout << "–°—Ä–Ķ–ī–Ĺ–Ķ–≥–ĺ–ī–ĺ–≤–ĺ–Ļ –Ņ—Ä–ĺ—Ü–Ķ–Ĺ—ā —É–ľ–Ķ—Ä—ą–ł—Ö –ĺ—ā –≥–ĺ–Ľ–ĺ–ī–į P = " << ( stats.deaths_percentage / 100 )  << "\n";
    std::cout << "–ö–ĺ–Ľ–ł—á–Ķ—Ā—ā–≤–ĺ –į–ļ—Ä–ĺ–≤ –∑–Ķ–ľ–Ľ–ł –Ĺ–į –ĺ–ī–Ĺ–ĺ–≥–ĺ –∂–ł—ā–Ķ–Ľ—Ź L = " << stats.land_per_man << "\n\n";

    if ( stats.deaths_percentage > STATS_LEVEL_BAD.deaths_percentage ||
        stats.land_per_man < STATS_LEVEL_BAD.land_per_man )
    {
        std::cout << "–ė–∑-–∑–į –≤–į—ą–Ķ–Ļ –Ĺ–Ķ–ļ–ĺ–ľ–Ņ–Ķ—ā–Ķ–Ĺ—ā–Ĺ–ĺ—Ā—ā–ł –≤ —É–Ņ—Ä–į–≤–Ľ–Ķ–Ĺ–ł–ł, –Ĺ–į—Ä–ĺ–ī —É—Ā—ā—Ä–ĺ–ł–Ľ –Ī—É–Ĺ—ā, –ł –ł–∑–≥–Ĺ–į–Ľ –≤–į—Ā –ł—Ö –≥–ĺ—Ä–ĺ–ī–į. –Ę–Ķ–Ņ–Ķ—Ä—Ć –≤—č –≤—č–Ĺ—É–∂–ī–Ķ–Ĺ—č –≤–Ľ–į—á–ł—ā—Ć –∂–į–Ľ–ļ–ĺ–Ķ —Ā—É—Č–Ķ—Ā—ā–≤–ĺ–≤–į–Ĺ–ł–Ķ –≤ –ł–∑–≥–Ĺ–į–Ĺ–ł–ł.\n";
        return;
    }

    if ( stats.deaths_percentage > STATS_LEVEL_OKAY.deaths_percentage ||
        stats.land_per_man < STATS_LEVEL_OKAY.land_per_man )
    {
        std::cout << "–í—č –Ņ—Ä–į–≤–ł–Ľ–ł –∂–Ķ–Ľ–Ķ–∑–Ĺ–ĺ–Ļ —Ä—É–ļ–ĺ–Ļ, –Ņ–ĺ–ī–ĺ–Ī–Ĺ–ĺ –Ě–Ķ—Ä–ĺ–Ĺ—É –ł –ė–≤–į–Ĺ—É –ď—Ä–ĺ–∑–Ĺ–ĺ–ľ—É.–Ě–į—Ä–ĺ–ī –≤–∑–ī–ĺ—Ö–Ĺ—É–Ľ —Ā –ĺ–Ī–Ľ–Ķ–≥—á–Ķ–Ĺ–ł–Ķ–ľ, –ł –Ĺ–ł–ļ—ā–ĺ –Ī–ĺ–Ľ—Ć—ą–Ķ –Ĺ–Ķ –∂–Ķ–Ľ–į–Ķ—ā –≤–ł–ī–Ķ—ā—Ć –≤–į—Ā –Ņ—Ä–į–≤–ł—ā–Ķ–Ľ–Ķ–ľ.\n";
        return;
    }

    if ( stats.deaths_percentage > STATS_LEVEL_GOOD.deaths_percentage ||
        stats.land_per_man < STATS_LEVEL_GOOD.land_per_man )
    {
        std::cout << "–í—č —Ā–Ņ—Ä–į–≤–ł–Ľ–ł—Ā—Ć –≤–Ņ–ĺ–Ľ–Ĺ–Ķ –Ĺ–Ķ–Ņ–Ľ–ĺ—Ö–ĺ, —É –≤–į—Ā, –ļ–ĺ–Ĺ–Ķ—á–Ĺ–ĺ, –Ķ—Ā—ā—Ć –Ĺ–Ķ–ī–ĺ–Ī—Ä–ĺ–∂–Ķ–Ľ–į—ā–Ķ–Ľ–ł, –Ĺ–ĺ –ľ–Ĺ–ĺ–≥–ł–Ķ —Ö–ĺ—ā–Ķ–Ľ–ł –Ī—č —É–≤–ł–ī–Ķ—ā—Ć –≤–į—Ā –≤–ĺ –≥–Ľ–į–≤–Ķ –≥–ĺ—Ä–ĺ–ī–į —Ā–Ĺ–ĺ–≤–į.\n";
        return;
    }

    std::cout << "–§–į–Ĺ—ā–į—Ā—ā–ł–ļ–į! –ö–į—Ä–Ľ –í–Ķ–Ľ–ł–ļ–ł–Ļ, –Ē–ł–∑—Ä–į—ć–Ľ–ł –ł –Ē–∂–Ķ—Ą—Ą–Ķ—Ä—Ā–ĺ–Ĺ –≤–ľ–Ķ—Ā—ā–Ķ –Ĺ–Ķ —Ā–Ņ—Ä–į–≤–ł–Ľ–ł—Ā—Ć –Ī—č –Ľ—É—á—ą–Ķ.\n";
}



/* Input */

bool ask_exit()
{
    char answer;
    std::cout << "–í–ĺ–∑–ľ–ĺ–∂–Ĺ–ĺ, –≤—č —Ö–ĺ—ā–ł—ā–Ķ –Ņ—Ä–Ķ—Ä–≤–į—ā—Ć –ł–≥—Ä—É? [y/n] ";
    std::cin >> answer;
    std::cout << "\n";
    return answer == 'y';
}

bool ask_load()
{
    char answer;
    std::cout << "–ė–ľ–Ķ–Ķ—ā—Ā—Ź —Ā–ĺ—Ö—Ä–į–Ĺ—Ď–Ĺ–Ĺ—č–Ļ –Ņ—Ä–ĺ–≥—Ä–Ķ—Ā—Ā. –ü—Ä–ĺ–ī–ĺ–Ľ–∂–ł—ā—Ć —Ā—É—Č–Ķ—Ā—ā–≤—É—é—Č—É—é –ł–≥—Ä—É? [y/n] ";
    std::cin >> answer;
    std::cout << "\n";
    return answer == 'y';
}


bool check_edict( const Edict& e, const GameState& s )
{
    const int land_delta = e.land_buy - e.land_sell;
    const int new_land = s.current.land + land_delta;
    if ( new_land <= 0 )
        return false;

    const int new_wheat = s.current.wheat
        - e.wheat_eat
        - s.land_price * land_delta
        - WHEAT_FOR_SEEDS_PER_ACRE * e.land_sow;
    if ( new_wheat < 0 )
        return false;

    const int land_could_sow = LAND_SOWN_BY_ONE_MAN * s.current.population;
    if ( e.land_sow > new_land || e.land_sow > land_could_sow )
        return false;

    return true;
}


Edict get_edict( const GameState& s )
{
    Edict result;

    std::cout << "–ß—ā–ĺ –Ņ–ĺ–∂–Ķ–Ľ–į–Ķ—ą—Ć, –Ņ–ĺ–≤–Ķ–Ľ–ł—ā–Ķ–Ľ—Ć?\n";

    while ( 1 )
    {
        while ( 1 )
        {
            std::cout << "–°–ļ–ĺ–Ľ—Ć–ļ–ĺ –į–ļ—Ä–ĺ–≤ –∑–Ķ–ľ–Ľ–ł –Ņ–ĺ–≤–Ķ–Ľ–Ķ–≤–į–Ķ—ą—Ć –ļ—É–Ņ–ł—ā—Ć? ";
            std::cin >> result.land_buy;
            if ( result.land_buy >= 0 )
                break;
            else
                std::cout << "–ú–ł–Ľ–ĺ—Ā—ā–ł–≤—č–Ļ –Ņ–ĺ–≤–Ķ–Ľ–ł—ā–Ķ–Ľ—Ć, –ľ—č –Ĺ–Ķ –Ņ–ĺ–Ĺ–ł–ľ–į–Ķ–ľ —á—ā–ĺ —ć—ā–ĺ –∑–į –ļ–ĺ–Ľ–ł—á–Ķ—Ā—ā–≤–ĺ\n";
        }

        while ( 1 )
        {
            std::cout << "–°–ļ–ĺ–Ľ—Ć–ļ–ĺ –į–ļ—Ä–ĺ–≤ –∑–Ķ–ľ–Ľ–ł –Ņ–ĺ–≤–Ķ–Ľ–Ķ–≤–į–Ķ—ą—Ć –Ņ—Ä–ĺ–ī–į—ā—Ć? ";
            std::cin >> result.land_sell;
            if ( result.land_sell >= 0 )
                break;
            else
                std::cout << "–ú–ł–Ľ–ĺ—Ā—ā–ł–≤—č–Ļ –Ņ–ĺ–≤–Ķ–Ľ–ł—ā–Ķ–Ľ—Ć, –ľ—č –Ĺ–Ķ –Ņ–ĺ–Ĺ–ł–ľ–į–Ķ–ľ —á—ā–ĺ —ć—ā–ĺ –∑–į –ļ–ĺ–Ľ–ł—á–Ķ—Ā—ā–≤–ĺ\n";
        }

        while ( 1 )
        {
            std::cout << "–°–ļ–ĺ–Ľ—Ć–ļ–ĺ –Ī—É—ą–Ķ–Ľ–Ķ–Ļ –Ņ—ą–Ķ–Ĺ–ł—Ü—č –Ņ–ĺ–≤–Ķ–Ľ–Ķ–≤–į–Ķ—ą—Ć —Ā—ä–Ķ—Ā—ā—Ć? ";
            std::cin >> result.wheat_eat;
            if ( result.wheat_eat >= 0 )
                break;
            else
                std::cout << "–ü–ĺ–≤–Ķ–Ľ–ł—ā–Ķ–Ľ—Ć, –ľ—č –Ĺ–Ķ —Ā–ľ–ĺ–∂–Ķ–ľ —Ā—ä–Ķ—Ā—ā—Ć —Ā—ā–ĺ–Ľ—Ć–ļ–ĺ –Ņ—ą–Ķ–Ĺ–ł—Ü—č\n";
        }

        while ( 1 )
        {
            std::cout << "–°–ļ–ĺ–Ľ—Ć–ļ–ĺ –į–ļ—Ä–ĺ–≤ –∑–Ķ–ľ–Ľ–ł –Ņ–ĺ–≤–Ķ–Ľ–Ķ–≤–į–Ķ—ą—Ć –∑–į—Ā–Ķ—Ź—ā—Ć? ";
            std::cin >> result.land_sow;
            if ( result.land_sow >= 0 )
                break;
            else
                std::cout << "–ü–ĺ–≤–Ķ–Ľ–ł—ā–Ķ–Ľ—Ć, –ľ—č –Ĺ–Ķ –∑–Ĺ–į–Ķ–ľ —á—ā–ĺ —ć—ā–ĺ –∑–į —á–ł—Ā–Ľ–ĺ";
        }

        if ( check_edict( result, s ) )
            break;
        else {
            std::cout << "–ě, –Ņ–ĺ–≤–Ķ–Ľ–ł—ā–Ķ–Ľ—Ć, –Ņ–ĺ—Č–į–ī–ł –Ĺ–į—Ā! –£ –Ĺ–į—Ā —ā–ĺ–Ľ—Ć–ļ–ĺ ";
            std::cout << s.current.population << man( s.current.population ) << ", ";
            std::cout << s.current.wheat << bushel( s.current.wheat ) << " –Ņ—ą–Ķ–Ĺ–ł—Ü—č, –ł ";
            std::cout << s.current.land << acre( s.current.land ) << " –∑–Ķ–ľ–Ľ–ł!\n\n";
        }
    }
    std::cout << "\n";
    std::cout << "–°–Ľ–Ķ–ī—É–Ķ–ľ –≤–į—ą–ł–ľ —É–ļ–į–∑–į–ľ...\n\n";
    return result;
}



/* Save & load game saves */

bool is_exist( const char* filename )
{
    std::ifstream infile( filename );
    return infile.good();
}

void save( const char* filename, const GameState& state, const Stats& stats )
{
    std::ofstream file;
    file.open( filename, std::ofstream::out | std::ofstream::trunc );
    file << state.round << ' ';
    file << state.current.land << ' ' << state.current.population << ' ' << state.current.wheat << ' ';
    file << state.income.land << ' ' << state.income.population << ' ' << state.income.wheat << ' ';
    file << state.waste.land << ' ' << state.waste.population << ' ' << state.waste.wheat << ' ';
    file << state.was_plague << ' ';
    file << state.wheat_from_acre << ' ';
    file << state.land_price << ' ';
    file << '\n';
    file << stats.deaths_percentage << ' ';
    file << stats.land_per_man << ' ';
    file << '\n';
    file.close();
}

GameSave load( const char* filename )
{
    GameState state;
    Stats stats;
    std::ifstream file;
    file.open( filename );
    file >> state.round;
    file >> state.current.land >> state.current.population >> state.current.wheat;
    file >> state.income.land >> state.income.population >> state.income.wheat;
    file >> state.waste.land >> state.waste.population >> state.waste.wheat;
    file >> state.was_plague;
    file >> state.wheat_from_acre;
    file >> state.land_price;
    file >> stats.deaths_percentage;
    file >> stats.land_per_man;
    file.close();
    return { state, stats };
}


/* Game process */

int new_land_price()
{
    std::default_random_engine random_engine( time( 0 ) );
    std::uniform_int_distribution<int> dist( MIN_LAND_PRICE, MAX_LAND_PRICE );
    return dist( random_engine );
}

RoundParams new_round_params()
{
    RoundParams result;
    std::default_random_engine random_engine( time( 0 ) );

    std::uniform_int_distribution<int> wheat_dist( MIN_WHEAT_FROM_ACRE, MAX_WHEAT_FROM_ACRE );
    result.wheat_from_acre = wheat_dist( random_engine );

    std::uniform_real_distribution<double> coef_dist( 0, MAX_RATS_–°OEFFICIENT );
    result.rat_coefficient = coef_dist( random_engine );

    std::uniform_real_distribution<double> percent_dist( 0, 100.0 );
    result.plague = ( percent_dist( random_engine ) ) <= PLAGUE_POSSIBILITY;

    return result;
}


GameState next_game_state( const GameState& s, const RoundParams& p, const Edict& e )
{
    GameState result;  
    result.round = s.round + 1;
    result.was_plague = p.plague;
    result.wheat_from_acre = p.wheat_from_acre;
    
    const int land_delta = e.land_buy - e.land_sell;
    result.income.land = e.land_buy;
    result.waste.land = e.land_sell;
    result.current.land = s.current.land + land_delta;
   
    result.income.wheat = p.wheat_from_acre * e.land_sow;
    const int new_wheat = s.current.wheat
        - e.wheat_eat
        - s.land_price * land_delta
        - WHEAT_FOR_SEEDS_PER_ACRE * e.land_sow
        + result.income.wheat;
    result.waste.wheat = trunc( new_wheat * p.rat_coefficient );
    result.current.wheat = new_wheat - result.waste.wheat;

    const int unfed = s.current.population - e.wheat_eat / WHEAT_DEMAND_PER_MAN;
    result.waste.population = ( unfed < 0 ) ? 0 : unfed;
    result.income.population = newcomers_count( result.waste.population, result.current.wheat, p.wheat_from_acre );
    result.current.population = s.current.population + result.income.population - result.waste.population;
    
    if ( p.plague )
        result.current.population = result.current.population / 2;

    result.land_price = new_land_price();

    return result;
}



const char* AUTOSAVE = "save.txt";

void game( const GameSave& s )
{
    GameState state = s.state;
    Stats stats = s.stats;
    bool isFailed = false;
    bool isFirstRound = true;

    while ( state.round <= ROUNDS_COUNT )
    {
        inform( state );

        if ( isFirstRound )
        {
            isFirstRound = false;
        }
        else
        {
            save( AUTOSAVE, state, stats );
            if ( ask_exit() )
                return;
        }
        
        Edict edict = get_edict( state );

        RoundParams params = new_round_params();

        const int previous_population = state.current.population;
        
        state = next_game_state( state, params, edict );

        Stats new_stats;
        new_stats.land_per_man = ( (double) state.current.land ) / state.current.population;
        const double current_deaths_percentage = ( (double) state.waste.population ) / previous_population;
        new_stats.deaths_percentage = (
            current_deaths_percentage
            + stats.deaths_percentage * ( state.round - 1 )
            ) / state.round;
        stats = new_stats;

        if ( current_deaths_percentage >= CRITICAL_DEATH_PERCENTAGE ||
            state.current.population <= 0 )
        {
            isFailed = true;
            break;
        }
    }
    std::cout << "\n–ė–≥—Ä–į –∑–į–ļ–ĺ–Ĺ—á–Ķ–Ĺ–į.\n\n";
    if ( isFailed )
        std::cout << "–í—č —Ā —ā—Ä–Ķ—Ā–ļ–ĺ–ľ –Ņ—Ä–ĺ–ł–≥—Ä–į–Ľ–ł, –Ĺ–į—Ā–Ķ–Ľ–Ķ–Ĺ–ł–Ķ —É–ľ–Ķ—Ä–Ľ–ĺ –ĺ—ā –≥–ĺ–Ľ–ĺ–ī–į.\n";
    else
    {
        finish_game( stats );
        std::remove( AUTOSAVE );
    }
}

void start_game()
{
    SetConsoleOutputCP( 65001 );
    std::cout << "–ü—Ä–ł–≤–Ķ—ā—Ā—ā–≤—É–Ķ–ľ —ā–Ķ–Ī—Ź, –ĺ –Ņ—Ä–į–≤–ł—ā–Ķ–Ľ—Ć!\n\n";

    if ( is_exist( AUTOSAVE ) )
    {
        if ( ask_load() )
        {
            GameSave saving_point = load( AUTOSAVE );
            game( saving_point );
            return;
        }
    }

    GameSave starting_point;
    starting_point.state.current = START_RESOURCES;
    starting_point.state.land_price = new_land_price();
    game( starting_point );
}



int main()
{
    start_game();
}