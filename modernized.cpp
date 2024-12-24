#include <iostream>
#include <array>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <limits>
#include <assert.h>
#include <string_view>
#include <optional>
#include <variant>
#include <fstream>
#include <sstream>
#include <tuple>
#include <utility>

#include "headers/fixed.hpp"
#include "headers/fast_fixed.hpp"
#include "headers/operators.hpp"

using namespace std;


constexpr size_t DEFAULT_N = 36, DEFAULT_M = 84;

#ifndef TYPES
#define TYPES FLOAT,FIXED(31,17),FAST_FIXED(25, 11),FIXED(32, 16),DOUBLE,FAST_FIXED(32, 16)
#endif

#ifndef SIZES
#define SIZES S(36,84), S(100,100)
#endif


struct GridDimension;
template<typename... Types> struct DataTypeCollection;
template<typename AllowedTypes, typename SelectedTypes> struct SimulationTypeResolver;
template<typename... Types> bool try_all_type_combinations(const string&, const string&, const string&, const GridDimension&, const string&);
template<typename P, typename V, typename VF> void executeSimulation(size_t, size_t, const string&);
template<typename T> std::string getTypeDescription();
pair<size_t, size_t> extractFixedPointParameters(const string&);
template<typename T> static bool matchesTypeSpecification(const string&);
GridDimension calculateFieldDimensions(const string&);
string loadFieldFromFile(const string&);
bool validateDataType(const string&);
string getCommandLineArgument(string_view, int, char**, string_view);
bool initializeAndRunSimulation(const string&, const string&, const string&, const GridDimension&, const string&);
GridDimension parse_size(const string&);
template<typename List> bool matches_size(const GridDimension&);


struct GridDimension {
    size_t n, m;
    constexpr GridDimension(size_t n_ = 0, size_t m_ = 0) : n(n_), m(m_) {}
};

template<typename NumericType, size_t N, size_t M>
struct DirectionalField {
    static constexpr std::array<pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
    array<NumericType, 4> v[N][M];

    NumericType &add(int x, int y, int dx, int dy, NumericType dv) {
        return get(x, y, dx, dy) += dv;
    }

    NumericType &get(int x, int y, int dx, int dy) {
        size_t i = ranges::find(deltas, pair(dx, dy)) - deltas.begin();
        assert(i < deltas.size());
        return v[x][y][i];
    }
};


template<
    typename PressureType,
    typename VelocityType,
    typename VFlowType,
    size_t N = DEFAULT_N,
    size_t M = DEFAULT_M
>
struct FluidSystemState {
    PressureType rho[256];
    PressureType p[N][M]{};
    PressureType old_p[N][M]{};

    char field[N][M + 1];
    int last_use[N][M]{};
    int UT{0};
    mt19937 rnd{1337};
    int dirs[N][M]{};

    DirectionalField<VelocityType, N, M> velocity{};
    DirectionalField<VFlowType, N, M> velocity_flow{};

    FluidSystemState(const string& field_content) {
        stringstream ss(field_content);
        string line;
        size_t row = 0;
        while (getline(ss, line) && row < N) {
            size_t col = 0;
            while (col < M && col < line.length()) {
                field[row][col] = line[col];
                col++;
            }
            field[row][col] = '\0';
            row++;
        }
    }
};

template<typename PressureType, typename VelocityType, typename VFlowType, size_t N = DEFAULT_N, size_t M = DEFAULT_M>
class FluidSimulator {
private:
    static constexpr size_t T = 1'000'000;
    static constexpr std::array<pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
    
    char field[N][M + 1];
    PressureType p[N][M]{}, old_p[N][M];
    int last_use[N][M]{};
    int UT = 0;
    mt19937 rnd{1337};
    PressureType rho[256];

    struct DirectionalField {
        array<VelocityType, 4> v[N][M]{};
        
        VelocityType& get(int x, int y, int dx, int dy) {
            size_t i = ranges::find(deltas, pair(dx, dy)) - deltas.begin();
            assert(i < deltas.size());
            return v[x][y][i];
        }

        VelocityType& add(int x, int y, int dx, int dy, VelocityType dv) {
            return get(x, y, dx, dy) += dv;
        }
    };

    DirectionalField velocity{}, velocity_flow{};
    int dirs[N][M]{};

    struct FluidParticleState {
        char type;
        PressureType cur_p;
        array<VelocityType, 4> v;

        void swap_with(FluidSimulator* sim, int x, int y) {
            swap(sim->field[x][y], type);
            swap(sim->p[x][y], cur_p);
            swap(sim->velocity.v[x][y], v);
        }
    };

    template<typename T>
    PressureType to_pressure(T value) {
        if constexpr (std::is_same_v<T, PressureType>) {
            return value;
        } else {
            return PressureType(value);
        }
    }

    template<typename T>
    VelocityType to_velocity(T value) {
        if constexpr (std::is_same_v<T, VelocityType>) {
            return value;
        } else {
            return VelocityType(value);
        }
    }

    template<typename T>
    VFlowType to_flow(T value) {
        if constexpr (std::is_same_v<T, VFlowType>) {
            return value;
        } else {
            return VFlowType(value);
        }
    }

    tuple<VelocityType, bool, pair<int, int>> propagate_flow(int x, int y, VelocityType lim) {
        last_use[x][y] = UT - 1;
        VelocityType ret = 0;
        for (auto [dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT) {
                auto cap = velocity.get(x, y, dx, dy);
                auto flow = velocity_flow.get(x, y, dx, dy);
                if (flow == cap) continue;
                
                auto vp = min(lim, cap - flow);
                if (last_use[nx][ny] == UT - 1) {
                    velocity_flow.add(x, y, dx, dy, vp);
                    last_use[x][y] = UT;
                    return {vp, true, {nx, ny}};
                }
                auto [t, prop, end] = propagate_flow(nx, ny, vp);
                ret += t;
                if (prop) {
                    velocity_flow.add(x, y, dx, dy, t);
                    last_use[x][y] = UT;
                    return {t, prop && end != pair(x, y), end};
                }
            }
        }
        last_use[x][y] = UT;
        return {ret, false, {0, 0}};
    }

    VelocityType random01() {
        return VelocityType(static_cast<double>(rnd() & ((1 << 16) - 1)) / (1 << 16));
    }

    void propagate_stop(int x, int y, bool force = false) {
        if (!force) {
            bool stop = true;
            for (auto [dx, dy] : deltas) {
                int nx = x + dx, ny = y + dy;
                if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) > VelocityType(0)) {
                    stop = false;
                    break;
                }
            }
            if (!stop) return;
        }
        last_use[x][y] = UT;
        for (auto [dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT || velocity.get(x, y, dx, dy) > VelocityType(0)) continue;
            propagate_stop(nx, ny);
        }
    }

    VelocityType move_prob(int x, int y) {
        VelocityType sum = 0;
        for (auto [dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT) continue;
            auto v = velocity.get(x, y, dx, dy);
            if (v < VelocityType(0)) continue;
            sum += v;
        }
        return sum;
    }

    bool propagate_move(int x, int y, bool is_first) {
        last_use[x][y] = UT - is_first;
        bool ret = false;
        int nx = -1, ny = -1;
        do {
            array<VelocityType, 4> tres;
            VelocityType sum = 0;
            for (size_t i = 0; i < deltas.size(); ++i) {
                auto [dx, dy] = deltas[i];
                int nx = x + dx, ny = y + dy;
                if (field[nx][ny] == '#' || last_use[nx][ny] == UT) {
                    tres[i] = sum;
                    continue;
                }
                auto v = velocity.get(x, y, dx, dy);
                if (v < VelocityType(0)) {
                    tres[i] = sum;
                    continue;
                }
                sum += v;
                tres[i] = sum;
            }

            if (sum == VelocityType(0)) break;

            VelocityType p = random01() * sum;
            size_t d = ranges::upper_bound(tres, p) - tres.begin();

            auto [dx, dy] = deltas[d];
            nx = x + dx;
            ny = y + dy;
            assert(velocity.get(x, y, dx, dy) > VelocityType(0) && field[nx][ny] != '#' && last_use[nx][ny] < UT);

            ret = (last_use[nx][ny] == UT - 1 || propagate_move(nx, ny, false));
        } while (!ret);

        last_use[x][y] = UT;
        for (auto [dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) < VelocityType(0)) {
                propagate_stop(nx, ny);
            }
        }
        if (ret && !is_first) {
            FluidParticleState pp{};
            pp.swap_with(this, x, y);
            pp.swap_with(this, nx, ny);
            pp.swap_with(this, x, y);
        }
        return ret;
    }

public:
    FluidSimulator(const FluidSystemState<PressureType, VelocityType, VFlowType, N, M>& state) {
        memcpy(field, state.field, sizeof(field));
        rho[' '] = PressureType(0.01);
        rho['.'] = PressureType(1000);
        
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                if (field[x][y] == '#') continue;
                for (auto [dx, dy] : deltas) {
                    dirs[x][y] += (field[x + dx][y + dy] != '#');
                }
            }
        }
    }

    void run() {
        PressureType g = PressureType(0.1);
        
        for (size_t i = 0; i < T; ++i) {
            PressureType total_delta_p = 0;
            
            // Apply external forces
            for (size_t x = 0; x < N; ++x) {
                for (size_t y = 0; y < M; ++y) {
                    if (field[x][y] == '#') continue;
                    if (field[x + 1][y] != '#')
                        velocity.add(x, y, 1, 0, VelocityType(g));
                }
            }

            // Apply forces from p
            memcpy(old_p, p, sizeof(p));
            for (size_t x = 0; x < N; ++x) {
                for (size_t y = 0; y < M; ++y) {
                    if (field[x][y] == '#') continue;
                    for (auto [dx, dy] : deltas) {
                        int nx = x + dx, ny = y + dy;
                        if (field[nx][ny] != '#' && old_p[nx][ny] < old_p[x][y]) {
                            auto delta_p = old_p[x][y] - old_p[nx][ny];
                            auto force = to_pressure(delta_p);
                            auto& contr = velocity.get(nx, ny, -dx, -dy);
                            if (to_pressure(contr) * rho[(int)field[nx][ny]] >= force) {
                                contr -= to_velocity(force / rho[(int)field[nx][ny]]);
                                continue;
                            }
                            force -= to_pressure(contr) * rho[(int)field[nx][ny]];
                            contr = 0;
                            velocity.add(x, y, dx, dy, to_velocity(force / rho[(int)field[x][y]]));
                            p[x][y] -= force / dirs[x][y];
                            total_delta_p -= force / dirs[x][y];
                        }
                    }
                }
            }

            // Make flow from velocities
            velocity_flow = {};
            bool prop = false;
            do {
                UT += 2;
                prop = false;
                for (size_t x = 0; x < N; ++x) {
                    for (size_t y = 0; y < M; ++y) {
                        if (field[x][y] != '#' && last_use[x][y] != UT) {
                            auto [t, local_prop, _] = propagate_flow(x, y, VelocityType(1));
                            if (t > VelocityType(0)) prop = true;
                        }
                    }
                }
            } while (prop);

            // Recalculate p with kinetic energy
            for (size_t x = 0; x < N; ++x) {
                for (size_t y = 0; y < M; ++y) {
                    if (field[x][y] == '#') continue;
                    for (auto [dx, dy] : deltas) {
                        auto old_v = velocity.get(x, y, dx, dy);
                        auto new_v = velocity_flow.get(x, y, dx, dy);
                        if (old_v > VelocityType(0)) {
                            assert(new_v <= old_v);
                            velocity.get(x, y, dx, dy) = new_v;
                            auto force = to_pressure(old_v - new_v) * rho[(int)field[x][y]];
                            if (field[x][y] == '.') force *= PressureType(0.8);
                            if (field[x + dx][y + dy] == '#') {
                                p[x][y] += force / dirs[x][y];
                                total_delta_p += force / dirs[x][y];
                            } else {
                                p[x + dx][y + dy] += force / dirs[x + dx][y + dy];
                                total_delta_p += force / dirs[x + dx][y + dy];
                            }
                        }
                    }
                }
            }

            UT += 2;
            prop = false;
            for (size_t x = 0; x < N; ++x) {
                for (size_t y = 0; y < M; ++y) {
                    if (field[x][y] != '#' && last_use[x][y] != UT) {
                        if (random01() < move_prob(x, y)) {
                            prop = true;
                            propagate_move(x, y, true);
                        } else {
                            propagate_stop(x, y, true);
                        }
                    }
                }
            }

            if (prop) {
                cout << "Tick " << i << ":\n";
                for (size_t x = 0; x < N; ++x) {
                    cout << field[x] << "\n";
                }
            }
        }
    }
};

template<typename T>
struct NumberTypeProperties {
    static T from_raw(int32_t x) { return T(x) / T(1 << 16); }
};

template<size_t N, size_t K>
struct NumberTypeProperties<Fixed<N,K>> {
    static Fixed<N,K> from_raw(typename Fixed<N,K>::DataStorage x) {
        return Fixed<N,K>::from_raw(x);
    }
};

template<typename T>
struct is_fixed : std::false_type {};

template<size_t N, size_t K>
struct is_fixed<Fixed<N,K>> : std::true_type {};

template<typename T>
inline constexpr bool is_fixed_v = is_fixed<T>::value;

template<typename T>
struct is_fast_fixed : std::false_type {};

template<size_t N, size_t K>
struct is_fast_fixed<FastFixed<N,K>> : std::true_type {};

template<typename T>
inline constexpr bool is_fast_fixed_v = is_fast_fixed<T>::value;

template<typename P, typename V, typename VF>
void executeSimulation(size_t rows, size_t cols, const string& fieldContent) {  
    FluidSystemState<P, V, VF, DEFAULT_N, DEFAULT_M> state(fieldContent);
    FluidSimulator<P, V, VF, DEFAULT_N, DEFAULT_M> simulator(state);
    simulator.run();
}

template<typename T>
std::string getTypeDescription() { 
    if constexpr (std::is_same_v<T, float>) {
        return "float";
    } else if constexpr (std::is_same_v<T, double>) {
        return "double";
    } else if constexpr (is_fixed_v<T>) {
        return "Fixed<" + std::to_string(T::bits) + "," + std::to_string(T::frac) + ">";
    } else if constexpr (is_fast_fixed_v<T>) {
        return "FastFixed<" + std::to_string(T::bits) + "," + std::to_string(T::frac) + ">";
    } else {
        return "unknown";
    }
}

pair<size_t, size_t> extractFixedPointParameters(const string& typeSpec) {  
    size_t start = typeSpec.find('(') + 1;
    size_t comma = typeSpec.find(',', start);
    size_t end = typeSpec.find(')', comma);
    
    size_t N = stoul(typeSpec.substr(start, comma - start));
    size_t K = stoul(typeSpec.substr(comma + 1, end - comma - 1));
    return {N, K};
}

template<typename T>
static bool matchesTypeSpecification(const string& typeSpec) { 
    if constexpr (std::is_same_v<T, float>) {
        return typeSpec == "FLOAT";
    } else if constexpr (std::is_same_v<T, double>) {
        return typeSpec == "DOUBLE";
    } else if constexpr (is_fixed_v<T>) {
        if (!typeSpec.starts_with("FIXED(")) return false;
        auto [bits, frac] = extractFixedPointParameters(typeSpec);
        return bits == T::bits && frac == T::frac;
    } else if constexpr (is_fast_fixed_v<T>) {
        if (!typeSpec.starts_with("FAST_FIXED(")) return false;
        auto [bits, frac] = extractFixedPointParameters(typeSpec);
        return bits == T::bits && frac == T::frac;
    }
    return false;
}

template<typename... Types>
struct DataTypeCollection {
    static constexpr size_t size = sizeof...(Types);
    template<size_t I>
    using type_at = typename std::tuple_element<I, std::tuple<Types...>>::type;
};

template<typename AllowedTypes, typename SelectedTypes>
struct SimulationTypeResolver {
    template<typename... Selected>
    static bool try_combinations(const string& p_type, const string& v_type, const string& v_flow_type,
                               const GridDimension& size, const string& field_content) {
        return try_all_p_types<0>(p_type, v_type, v_flow_type, size, field_content);
    }

private:
    template<size_t I>
    static bool try_all_p_types(const string& p_type, const string& v_type, const string& v_flow_type,
                               const GridDimension& size, const string& field_content) {
        if constexpr (I >= AllowedTypes::size) {
            return false;
        } else {
            using P = typename AllowedTypes::template type_at<I>;
            return try_with_p_type<P>(p_type, v_type, v_flow_type, size, field_content) ||
                   try_all_p_types<I + 1>(p_type, v_type, v_flow_type, size, field_content);
        }
    }

    template<typename P>
    static bool try_with_p_type(const string& p_type, const string& v_type, const string& v_flow_type,
                               const GridDimension& size, const string& field_content) {
        if (!matchesTypeSpecification<P>(p_type)) return false;
        return try_all_v_types<P, 0>(p_type, v_type, v_flow_type, size, field_content);
    }

    template<typename P, size_t I>
    static bool try_all_v_types(const string& p_type, const string& v_type, const string& v_flow_type,
                               const GridDimension& size, const string& field_content) {
        if constexpr (I >= AllowedTypes::size) {
            return false;
        } else {
            using V = typename AllowedTypes::template type_at<I>;
            return try_with_v_type<P, V>(p_type, v_type, v_flow_type, size, field_content) ||
                   try_all_v_types<P, I + 1>(p_type, v_type, v_flow_type, size, field_content);
        }
    }

    template<typename P, typename V>
    static bool try_with_v_type(const string& p_type, const string& v_type, const string& v_flow_type,
                               const GridDimension& size, const string& field_content) {
        if (!matchesTypeSpecification<V>(v_type)) return false;
        return try_all_vf_types<P, V, 0>(p_type, v_type, v_flow_type, size, field_content);
    }

    template<typename P, typename V, size_t I>
    static bool try_all_vf_types(const string& p_type, const string& v_type, const string& v_flow_type,
                                const GridDimension& size, const string& field_content) {
        if constexpr (I >= AllowedTypes::size) {
            return false;
        } else {
            using VF = typename AllowedTypes::template type_at<I>;
            return try_with_vf_type<P, V, VF>(p_type, v_type, v_flow_type, size, field_content) ||
                   try_all_vf_types<P, V, I + 1>(p_type, v_type, v_flow_type, size, field_content);
        }
    }

    template<typename P, typename V, typename VF>
    static bool try_with_vf_type(const string& p_type, const string& v_type, const string& v_flow_type,
                                const GridDimension& size, const string& field_content) {
        if (!matchesTypeSpecification<VF>(v_flow_type)) return false;
        executeSimulation<P, V, VF>(size.n, size.m, field_content);
        return true;
    }
};

template<typename... Types>
bool try_all_type_combinations(const string& p_type, const string& v_type, const string& v_flow_type,
                             const GridDimension& size, const string& field_content) {
    return SimulationTypeResolver<DataTypeCollection<Types...>, DataTypeCollection<>>::try_combinations(p_type, v_type, v_flow_type, size, field_content);
}

string loadFieldFromFile(const string& filepath) {  
    ifstream file(filepath);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file: " + filepath);
    }
    
    string content, line;
    while (getline(file, line)) {
        content += line + '\n';
    }
    return content;
}

bool validateDataType(const string& typeName) { 
    if (typeName == "FLOAT" || typeName == "DOUBLE") return true;
    
    if (typeName.starts_with("FIXED(") || typeName.starts_with("FAST_FIXED(")) {
        size_t pos = typeName.find(',');
        if (pos == string::npos) return false;
        
        size_t end_pos = typeName.find(')', pos);
        if (end_pos == string::npos) return false;
        
        return true;
    }
    
    return false;
}

string getCommandLineArgument(string_view argName, int argc, char** argv, string_view defaultValue) { 
    std::string searchArg = std::string(argName);
    
    for (int i = 1; i < argc; ++i) {
        std::string currentArg(argv[i]);
        if (currentArg.starts_with(searchArg + "=")) {
            std::string value = currentArg.substr(searchArg.length() + 1);
            if (value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.length() - 2);
            }
            replace(value.begin(), value.end(), '<', '(');
            replace(value.begin(), value.end(), '>', ')');
            return value;
        }
    }
    return string(defaultValue);
}

template<GridDimension... Dims>
struct GridDimensionSet {
    static constexpr size_t size = sizeof...(Dims);
    template<size_t I>
    static constexpr GridDimension get() {
        constexpr GridDimension arr[] = {Dims...};
        return arr[I];
    }
};

template<typename List, size_t I = 0>
constexpr bool matches_size_impl(const GridDimension& size) {
    if constexpr (I >= List::size) {
        return false;
    } else {
        return (List::template get<I>().n == size.n && List::template get<I>().m == size.m) ||
               matches_size_impl<List, I + 1>(size);
    }
}

template<typename List>
bool matches_size(const GridDimension& size) {
    return matches_size_impl<List>(size);
}

bool initializeAndRunSimulation(const string& pressureType, 
                              const string& velocityType, 
                              const string& flowType, 
                              const GridDimension& dimensions, 
                              const string& fieldContent) {
    try {

        #define S(N, M) GridDimension(N, M)
        using SizesListType = GridDimensionSet<SIZES>;
        #undef S
        
        if (!matches_size<SizesListType>(dimensions)) {
            cerr << "Error: Unsupported size" << endl;
            return false;
        }

        #define FLOAT float
        #define DOUBLE double
        #define FIXED(N, K) Fixed<N, K>
        #define FAST_FIXED(N, K) FastFixed<N, K>
        if (!try_all_type_combinations<TYPES>(pressureType, velocityType, flowType, dimensions, fieldContent)) {
            cerr << "Error: No matching type combination found" << endl;
            return false;
        }
        return true;
    }
    catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return false;
    }
}

GridDimension calculateFieldDimensions(const string& fieldContent) { 
    size_t n = 0, m = 0;
    stringstream ss(fieldContent);
    string line;
    
    while (getline(ss, line)) {
        if (m == 0) m = line.length();
        else if (line.length() != m) {
            throw runtime_error("Invalid field format: lines have different lengths");
        }
        n++;
    }
    
    if (n == 0 || m == 0) {
        throw runtime_error("Empty field");
    }
    
    if (n > DEFAULT_N || m > DEFAULT_M) {
        throw runtime_error("Field size " + to_string(n) + "x" + to_string(m) + 
                          " exceeds maximum allowed size " + to_string(DEFAULT_N) + 
                          "x" + to_string(DEFAULT_M));
    }
    
    return GridDimension(n, m);
}

GridDimension parse_size(const string& size_str) {
    if (!size_str.starts_with("S(")) {
        throw std::runtime_error("Size must start with S(");
    }
    
    size_t start = 2;
    size_t comma = size_str.find(',', start);
    if (comma == string::npos) {
        throw std::runtime_error("Invalid size format: missing comma");
    }
    
    size_t end = size_str.find(')', comma);
    if (end == string::npos) {
        throw std::runtime_error("Invalid size format: missing )");
    }
    
    size_t n = stoul(size_str.substr(start, comma - start));
    size_t m = stoul(size_str.substr(comma + 1, end - comma - 1));
    
    return GridDimension(n, m);
}

int main(int argc, char** argv) {
    string pressureType = getCommandLineArgument("--p-type", argc, argv, "FLOAT");
    string velocityType = getCommandLineArgument("--v-type", argc, argv, "FLOAT");
    string flowType = getCommandLineArgument("--v-flow-type", argc, argv, "FLOAT");
    string fieldFilePath = getCommandLineArgument("--field", argc, argv, "../field.txt");
    
    string fieldContent;
    try {
        fieldContent = loadFieldFromFile(fieldFilePath);
    } catch (const exception& e) {
        cerr << "Error reading field file: " << e.what() << endl;
        return 1;
    }

    GridDimension dimensions;
    try {
        dimensions = calculateFieldDimensions(fieldContent);
    } catch (const exception& e) {
        cerr << "Error validating field: " << e.what() << endl;
        return 1;
    }

    if (!validateDataType(pressureType)) {
        cerr << "Invalid pressure type: " << pressureType << endl;
        return 1;
    }
    if (!validateDataType(velocityType)) {
        cerr << "Invalid velocity type: " << velocityType << endl;
        return 1;
    }
    if (!validateDataType(flowType)) {
        cerr << "Invalid flow type: " << flowType << endl;
        return 1;
    }

    if (!initializeAndRunSimulation(pressureType, velocityType, flowType, dimensions, fieldContent)) {
        cerr << "Failed to create simulation" << endl;
        return 1;
    }
    
    return 0;
}