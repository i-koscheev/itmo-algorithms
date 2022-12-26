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
constexpr double MAX_RATS_СOEFFICIENT = 0.07;
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

const char* const words_man[] = { " человек", " человек", " человекa" };
const char* const words_acre[] = { " акров", " акр", " акра" };
const char* const words_bushel[] = { " бушелей", " бушель", " бушеля" };

const char* man( int k ) { return numbered_word( words_man, k ); }
const char* acre( int k ) { return numbered_word( words_acre, k ); }
const char* bushel( int k ) { return numbered_word( words_bushel, k ); }


/* Output */

void inform( const GameState& s )
{
    std::cout << "Мой повелитель, соизволь поведать тебе \n";
    std::cout << "в году " << s.round << " твоего высочайшего правления\n";

    if ( s.waste.population > 0 )
    {
        std::cout << s.waste.population << man( s.waste.population ) << " умерли с голоду";
        if ( s.income.population > 0 )
            std::cout << ", и ";
        else
            std::cout << ".\n";
    }
    if ( s.income.population > 0 )
        std::cout << s.income.population << man( s.income.population ) << " прибыли в наш великий город.\n";

    if ( s.was_plague )
        std::cout << "Чума уничтожила половину населения.\n";

    std::cout << "Население города составляет " << s.current.population << man( s.current.population ) << ".\n";

    if ( s.income.wheat > 0 )
    {
        std::cout << "Мы собрали " << s.income.wheat << bushel( s.income.wheat ) << " пшеницы, ";
        std::cout << s.wheat_from_acre << bushel( s.wheat_from_acre ) << " с акра.";
        std::cout << "\n";
    }
    if ( s.waste.wheat > 0 )
    {
        std::cout << "Крысы истребили " << s.waste.wheat << bushel( s.waste.wheat ) << " пшеницы,";
        std::cout << " оставив " << s.current.wheat << bushel( s.current.wheat ) << " в амбарах.\n";
    }
    else
        std::cout << "В амбарах осталось " << s.current.wheat << bushel( s.current.wheat ) << " пшеницы.\n";

    std::cout << "Город сейчас занимает " << s.current.land << acre( s.current.land ) << ".\n";

    std::cout << "1 акр земли стоит сейчас " << s.land_price << bushel( s.land_price ) << ".\n\n";
}


void finish_game( const Stats& stats )
{
    std::cout << "Статистика игры:\n";
    std::cout << "Среднегодовой процент умерших от голода P = " << ( stats.deaths_percentage / 100 )  << "\n";
    std::cout << "Количество акров земли на одного жителя L = " << stats.land_per_man << "\n\n";

    if ( stats.deaths_percentage > STATS_LEVEL_BAD.deaths_percentage ||
        stats.land_per_man < STATS_LEVEL_BAD.land_per_man )
    {
        std::cout << "Из-за вашей некомпетентности в управлении, народ устроил бунт, и изгнал вас их города. Теперь вы вынуждены влачить жалкое существование в изгнании.\n";
        return;
    }

    if ( stats.deaths_percentage > STATS_LEVEL_OKAY.deaths_percentage ||
        stats.land_per_man < STATS_LEVEL_OKAY.land_per_man )
    {
        std::cout << "Вы правили железной рукой, подобно Нерону и Ивану Грозному.Народ вздохнул с облегчением, и никто больше не желает видеть вас правителем.\n";
        return;
    }

    if ( stats.deaths_percentage > STATS_LEVEL_GOOD.deaths_percentage ||
        stats.land_per_man < STATS_LEVEL_GOOD.land_per_man )
    {
        std::cout << "Вы справились вполне неплохо, у вас, конечно, есть недоброжелатели, но многие хотели бы увидеть вас во главе города снова.\n";
        return;
    }

    std::cout << "Фантастика! Карл Великий, Дизраэли и Джефферсон вместе не справились бы лучше.\n";
}



/* Input */

bool ask_exit()
{
    char answer;
    std::cout << "Возможно, вы хотите прервать игру? [y/n] ";
    std::cin >> answer;
    std::cout << "\n";
    return answer == 'y';
}

bool ask_load()
{
    char answer;
    std::cout << "Имеется сохранённый прогресс. Продолжить существующую игру? [y/n] ";
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

    std::cout << "Что пожелаешь, повелитель?\n";

    while ( 1 )
    {
        while ( 1 )
        {
            std::cout << "Сколько акров земли повелеваешь купить? ";
            std::cin >> result.land_buy;
            if ( result.land_buy >= 0 )
                break;
            else
                std::cout << "Милостивый повелитель, мы не понимаем что это за количество\n";
        }

        while ( 1 )
        {
            std::cout << "Сколько акров земли повелеваешь продать? ";
            std::cin >> result.land_sell;
            if ( result.land_sell >= 0 )
                break;
            else
                std::cout << "Милостивый повелитель, мы не понимаем что это за количество\n";
        }

        while ( 1 )
        {
            std::cout << "Сколько бушелей пшеницы повелеваешь съесть? ";
            std::cin >> result.wheat_eat;
            if ( result.wheat_eat >= 0 )
                break;
            else
                std::cout << "Повелитель, мы не сможем съесть столько пшеницы\n";
        }

        while ( 1 )
        {
            std::cout << "Сколько акров земли повелеваешь засеять? ";
            std::cin >> result.land_sow;
            if ( result.land_sow >= 0 )
                break;
            else
                std::cout << "Повелитель, мы не знаем что это за число";
        }

        if ( check_edict( result, s ) )
            break;
        else {
            std::cout << "О, повелитель, пощади нас! У нас только ";
            std::cout << s.current.population << man( s.current.population ) << ", ";
            std::cout << s.current.wheat << bushel( s.current.wheat ) << " пшеницы, и ";
            std::cout << s.current.land << acre( s.current.land ) << " земли!\n\n";
        }
    }
    std::cout << "\n";
    std::cout << "Следуем вашим указам...\n\n";
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

    std::uniform_real_distribution<double> coef_dist( 0, MAX_RATS_СOEFFICIENT );
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
    std::cout << "\nИгра закончена.\n\n";
    if ( isFailed )
        std::cout << "Вы с треском проиграли, население умерло от голода.\n";
    else
    {
        finish_game( stats );
        std::remove( AUTOSAVE );
    }
}

void start_game()
{
    SetConsoleOutputCP( 65001 );
    std::cout << "Приветствуем тебя, о правитель!\n\n";

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