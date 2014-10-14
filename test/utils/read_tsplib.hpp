//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
// Grzegorz Prusak
#ifndef PAAL_READ_TSPLIB_HPP
#define PAAL_READ_TSPLIB_HPP

#include "paal/data_structures/metric/basic_metrics.hpp"

#include <cassert>
#include <cmath>
#include <istream>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

template <typename T> inline T sqr(T a) { return a * a; }

namespace read_tsplib {

struct TSPLIB_Matrix {
    typedef long long DistanceType;
    typedef paal::data_structures::array_metric<int> metric;
    TSPLIB_Matrix() : size_(0) {}
    typedef double value_type;
    typedef int (*Dist)(double, double);
    int operator()(size_t i, size_t j) const {
        return dist_ ? dist_(X[i] - X[j], Y[i] - Y[j]) : mtx(i, j);
    }

    static int eucl_dist(double xd, double yd) {
        return int(.5 + sqrt(xd * xd + yd * yd));
    }
    static int ceil_dist(double xd, double yd) {
        return ceil(sqrt(xd * xd + yd * yd));
    }
    static int att_dist(double xd, double yd) {
        return ceil(sqrt((xd * xd + yd * yd) / 10));
    }

    void resize(size_t _size, Dist _dist = 0) {
        if ((dist_ = _dist)) {
            X.reset(new double[_size]);
            // badalloc below will invalidate the object
            Y.reset(new double[_size]);
            mtx = metric(0);
        } else {
            mtx = metric(_size);
            X.reset();
            Y.reset();
        }
        size_ = _size;
    }

    size_t size() const { return size_; }

    Dist dist_;
    size_t size_;
    metric mtx;
    std::unique_ptr<double[]> X, Y;
};

typedef std::vector<std::stringstream> VSS;
inline void push(VSS &vss, size_t i) {}
template <typename T, typename... Args>
inline void push(VSS &vss, size_t i, const T &v, Args... args) {
    vss[i] << v;
    push(vss, i + 1, args...);
}

template <typename... Args>
inline std::string format(std::string format_str, Args... args) {
    VSS vss(sizeof...(Args));
    push(vss, 0, args...);
    std::stringstream ss;
    int i = 0;
    for (char c : format_str)
        if (c != '%')
            ss << c;
        else
            ss << vss.at(i++).str();
    return ss.str();
}

struct TSPLIB_Directory {
    std::string dir;
    std::ifstream index;
    TSPLIB_Directory(const std::string &_dir,
                     const std::string &indexFName = "/index")
        : dir(_dir), index(dir + indexFName) {
        assert(index);
    }

    bool get_graph(std::string &fname, float &opt) {
        std::string header;
        if (get_header(index, header) >> opt) {
            fname = dir + "/" + header + ".tsp";
            return true;
        }
        return false;
    }

    struct Graph {
        Graph(std::istream &_is, const std::string &_desc = "",
              double _optimal_fitness = -1)
            : is(_is), optimal_fitness(_optimal_fitness), desc(_desc) {}
        std::istream &is;
        double optimal_fitness;
        const std::string desc;

        // http://www.iwr.uni-heidelberg.de/groups/comopt/software/TSPLIB95/TSPFAQ.html
        static double geo_rad(double x) {
            const double PI = 3.141592;
            int deg = x;
            return PI * (int(deg) + 5 * (x - deg) / 3) / 180;
        }

        static double geo_dist(double longitude1, double latitude1,
                               double longitude2, double latitude2) {
            const double RRR = 6378.388;

            double q1 = cos(longitude1 - longitude2);
            double q2 = cos(latitude1 - latitude2);
            double q3 = cos(latitude1 + latitude2);
            return int(RRR * acos(.5 * ((1. + q1) * q2 - (1. - q1) * q3)) +
                       1.0);
        }

        void load(TSPLIB_Matrix &m) {
            expect_header(is, "DIMENSION");
            size_t n;
            assert(is >> n);
            expect_header(is, "EDGE_WEIGHT_TYPE");
            std::string ewt;
            assert(is >> ewt);
            if (ewt == "EXPLICIT") {
                expect_header(is, "EDGE_WEIGHT_FORMAT");
                std::string ewf;
                assert(is >> ewf);
                m.resize(n);
                expect(is, "EDGE_WEIGHT_SECTION");
                if (ewf == "FULL_MATRIX")
                    for (size_t i = 0; i < n; ++i)
                        for (size_t j = 0; j < n; ++j)
                            assert(is >> m.mtx(i, j));
                else if (ewf == "UPPER_ROW") {
                    for (size_t i = 0; i < n; ++i) m.mtx(i, i) = 0;
                    for (size_t i = 0; i < n; ++i)
                        for (size_t j = i + 1; j < n; ++j) {
                            int d;
                            assert(is >> d);
                            m.mtx(i, j) = m.mtx(j, i) = d;
                        }
                } else if (ewf == "LOWER_DIAG_ROW")
                    for (size_t i = 0; i < n; ++i)
                        for (size_t j = 0; j <= i; ++j) {
                            int d;
                            assert(is >> d);
                            m.mtx(i, j) = m.mtx(j, i) = d;
                        }
                else if (ewf == "UPPER_DIAG_ROW")
                    for (size_t i = 0; i < n; ++i)
                        for (size_t j = i; j < n; ++j) {
                            int d;
                            assert(is >> d);
                            m.mtx(i, j) = m.mtx(j, i) = d;
                        }
                else
                    throw std::runtime_error(
                        format("EDGE_WEIGHT_FORMAT % is unimplemented", ewf));
            } else if (ewt == "GEO") {
                expect(is, "NODE_COORD_SECTION");
                std::vector<double> X(n), Y(n);
                int _;
                for (size_t i = 0; i < n; ++i) assert(is >> _ >> X[i] >> Y[i]);
                for (double &x : X) x = geo_rad(x);
                for (double &y : Y) y = geo_rad(y);
                m.resize(n);
                for (size_t i = 0; i < n; ++i)
                    for (size_t j = i; j < n; ++j) {
                        m.mtx(i, j) = m.mtx(j, i) =
                            geo_dist(Y[i], X[i], Y[j], X[j]);
                    }
            } else {
                if (ewt == "EUC_2D")
                    m.resize(n, m.eucl_dist);
                else if (ewt == "CEIL_2D")
                    m.resize(n, m.ceil_dist);
                else if (ewt == "ATT")
                    m.resize(n, m.att_dist);
                else
                    throw std::runtime_error(
                        format("EDGE_WEIGHT_TYPE % is unimplemented", ewt));
                expect(is, "NODE_COORD_SECTION");

                int _;
                for (size_t i = 0; i < n; ++i)
                    assert(is >> _ >> m.X[i] >> m.Y[i]);
            }
        }
    };

    static void expect(std::istream &is, const std::string &pattern) {
        std::string s;
        while (1) {
            assert(is >> s);
            if (s == pattern) return;
        }
    }
    static std::istream &get_header(std::istream &is, std::string &header) {
        std::getline(is, header, ':');
        std::stringstream ss(header);
        ss >> header;
        return is;
    }

    static void expect_header(std::istream &is, const std::string &header) {
        std::string s;
        while (1) {
            assert(get_header(is, s));
            if (s == header) return;
            assert(std::getline(is, s));
        }
    }
};
}

#endif // PAAL_READ_TSPLIB_HPP
