#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

constexpr double PI = 3.14159265358979323846;

struct Vec3 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    Vec3() = default;
    Vec3(double X, double Y, double Z) : x(X), y(Y), z(Z) {}
    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }
};

static double dot(const Vec3& a, const Vec3& b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static Vec3 cross(const Vec3& a, const Vec3& b) {
    return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}
static double norm(const Vec3& v) { return std::sqrt(dot(v, v)); }
static Vec3 normalize(const Vec3& v) {
    const double n = norm(v);
    if (n < 1e-12) return {0,0,0};
    return {v.x/n, v.y/n, v.z/n};
}
static Vec3 lerp(const Vec3& a, const Vec3& b, double t) { return a*(1.0-t) + b*t; }

struct Image {
    int width;
    int height;
    std::vector<unsigned char> pix;
    Image(int w, int h) : width(w), height(h), pix(static_cast<size_t>(w*h*3), 255) {}
    void clear(unsigned char r, unsigned char g, unsigned char b) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const size_t id = static_cast<size_t>((y*width + x)*3);
                pix[id+0] = r; pix[id+1] = g; pix[id+2] = b;
            }
        }
    }
    void set_pixel(int x, int y, int r, int g, int b) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        const size_t id = static_cast<size_t>((y*width + x)*3);
        pix[id+0] = static_cast<unsigned char>(std::clamp(r,0,255));
        pix[id+1] = static_cast<unsigned char>(std::clamp(g,0,255));
        pix[id+2] = static_cast<unsigned char>(std::clamp(b,0,255));
    }
    void blend_pixel(int x, int y, int r, int g, int b, double alpha) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        const size_t id = static_cast<size_t>((y*width + x)*3);
        pix[id+0] = static_cast<unsigned char>(std::clamp((1-alpha)*pix[id+0] + alpha*r, 0.0, 255.0));
        pix[id+1] = static_cast<unsigned char>(std::clamp((1-alpha)*pix[id+1] + alpha*g, 0.0, 255.0));
        pix[id+2] = static_cast<unsigned char>(std::clamp((1-alpha)*pix[id+2] + alpha*b, 0.0, 255.0));
    }
    void write_ppm(const std::string& filename) const {
        std::ofstream out(filename, std::ios::binary);
        if (!out) throw std::runtime_error("Could not write " + filename);
        out << "P6\n" << width << " " << height << "\n255\n";
        out.write(reinterpret_cast<const char*>(pix.data()), static_cast<std::streamsize>(pix.size()));
    }
};

// Minimal 5x7 bitmap font for the title and equations.
static const std::array<const char*,7>& glyph(char c) {
    static const std::array<const char*,7> blank = {"00000","00000","00000","00000","00000","00000","00000"};
    static const std::array<const char*,7> A={"01110","10001","10001","11111","10001","10001","10001"};
    static const std::array<const char*,7> B={"11110","10001","10001","11110","10001","10001","11110"};
    static const std::array<const char*,7> C={"01111","10000","10000","10000","10000","10000","01111"};
    static const std::array<const char*,7> D={"11110","10001","10001","10001","10001","10001","11110"};
    static const std::array<const char*,7> E={"11111","10000","10000","11110","10000","10000","11111"};
    static const std::array<const char*,7> F={"11111","10000","10000","11110","10000","10000","10000"};
    static const std::array<const char*,7> G={"01111","10000","10000","10011","10001","10001","01111"};
    static const std::array<const char*,7> H={"10001","10001","10001","11111","10001","10001","10001"};
    static const std::array<const char*,7> I={"11111","00100","00100","00100","00100","00100","11111"};
    static const std::array<const char*,7> J={"00111","00010","00010","00010","10010","10010","01100"};
    static const std::array<const char*,7> K={"10001","10010","10100","11000","10100","10010","10001"};
    static const std::array<const char*,7> L={"10000","10000","10000","10000","10000","10000","11111"};
    static const std::array<const char*,7> M={"10001","11011","10101","10101","10001","10001","10001"};
    static const std::array<const char*,7> N={"10001","11001","10101","10011","10001","10001","10001"};
    static const std::array<const char*,7> O={"01110","10001","10001","10001","10001","10001","01110"};
    static const std::array<const char*,7> P={"11110","10001","10001","11110","10000","10000","10000"};
    static const std::array<const char*,7> Q={"01110","10001","10001","10001","10101","10010","01101"};
    static const std::array<const char*,7> R={"11110","10001","10001","11110","10100","10010","10001"};
    static const std::array<const char*,7> S={"01111","10000","10000","01110","00001","00001","11110"};
    static const std::array<const char*,7> T={"11111","00100","00100","00100","00100","00100","00100"};
    static const std::array<const char*,7> U={"10001","10001","10001","10001","10001","10001","01110"};
    static const std::array<const char*,7> V={"10001","10001","10001","10001","10001","01010","00100"};
    static const std::array<const char*,7> W={"10001","10001","10001","10101","10101","10101","01010"};
    static const std::array<const char*,7> X={"10001","10001","01010","00100","01010","10001","10001"};
    static const std::array<const char*,7> Y={"10001","10001","01010","00100","00100","00100","00100"};
    static const std::array<const char*,7> Z={"11111","00001","00010","00100","01000","10000","11111"};
    static const std::array<const char*,7> zero={"01110","10001","10011","10101","11001","10001","01110"};
    static const std::array<const char*,7> one={"00100","01100","00100","00100","00100","00100","01110"};
    static const std::array<const char*,7> two={"01110","10001","00001","00010","00100","01000","11111"};
    static const std::array<const char*,7> three={"11110","00001","00001","01110","00001","00001","11110"};
    static const std::array<const char*,7> four={"00010","00110","01010","10010","11111","00010","00010"};
    static const std::array<const char*,7> five={"11111","10000","10000","11110","00001","00001","11110"};
    static const std::array<const char*,7> six={"01110","10000","10000","11110","10001","10001","01110"};
    static const std::array<const char*,7> seven={"11111","00001","00010","00100","01000","01000","01000"};
    static const std::array<const char*,7> eight={"01110","10001","10001","01110","10001","10001","01110"};
    static const std::array<const char*,7> nine={"01110","10001","10001","01111","00001","00001","01110"};
    static const std::array<const char*,7> minus={"00000","00000","00000","11111","00000","00000","00000"};
    static const std::array<const char*,7> plus={"00000","00100","00100","11111","00100","00100","00000"};
    static const std::array<const char*,7> equal={"00000","00000","11111","00000","11111","00000","00000"};
    static const std::array<const char*,7> slash={"00001","00010","00010","00100","01000","01000","10000"};
    static const std::array<const char*,7> lpar={"00010","00100","01000","01000","01000","00100","00010"};
    static const std::array<const char*,7> rpar={"01000","00100","00010","00010","00010","00100","01000"};
    static const std::array<const char*,7> dotc={"00000","00000","00000","00000","00000","01100","01100"};
    static const std::array<const char*,7> colon={"00000","01100","01100","00000","01100","01100","00000"};
    c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    switch(c){
        case 'A':return A; case 'B':return B; case 'C':return C; case 'D':return D; case 'E':return E; case 'F':return F; case 'G':return G; case 'H':return H; case 'I':return I; case 'J':return J; case 'K':return K; case 'L':return L; case 'M':return M; case 'N':return N; case 'O':return O; case 'P':return P; case 'Q':return Q; case 'R':return R; case 'S':return S; case 'T':return T; case 'U':return U; case 'V':return V; case 'W':return W; case 'X':return X; case 'Y':return Y; case 'Z':return Z;
        case '0':return zero; case '1':return one; case '2':return two; case '3':return three; case '4':return four; case '5':return five; case '6':return six; case '7':return seven; case '8':return eight; case '9':return nine;
        case '-':return minus; case '+':return plus; case '=':return equal; case '/':return slash; case '(':return lpar; case ')':return rpar; case '.':return dotc; case ':':return colon; default:return blank;
    }
}

static void draw_text(Image& img, int x, int y, const std::string& text, int scale, int r, int g, int b) {
    int cx = x;
    for (char c : text) {
        const auto& gl = glyph(c);
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if (gl[row][col] == '1') {
                    for (int yy = 0; yy < scale; ++yy)
                        for (int xx = 0; xx < scale; ++xx)
                            img.set_pixel(cx + col*scale + xx, y + row*scale + yy, r,g,b);
                }
            }
        }
        cx += 6*scale;
    }
}

struct Projected { double x=0, y=0, depth=0; };
struct Triangle { Vec3 a,b,c; Vec3 normal; double potential; };
struct Segment { Vec3 a,b; double radius; };

class Renderer {
public:
    Renderer(int w, int h) : w_(w), h_(h), zbuffer_(static_cast<size_t>(w*h), -1e9) {}
    void reset() { std::fill(zbuffer_.begin(), zbuffer_.end(), -1e9); }
    Projected project(Vec3 p, double angle) const {
        const double ca = std::cos(angle), sa = std::sin(angle);
        const double xr = ca*p.x + sa*p.z;
        const double zr = -sa*p.x + ca*p.z;
        const double yr = p.y;
        const double tilt = 0.42;
        const double ct = std::cos(tilt), st = std::sin(tilt);
        const double y2 = ct*yr - st*zr;
        const double z2 = st*yr + ct*zr;
        const double perspective = 1.72 / (2.70 - z2);
        const double scale = 318.0;
        return {0.52*w_ + xr*perspective*scale, 0.56*h_ - y2*perspective*scale, z2};
    }
    void draw_disk(Image& img, const Projected& q, int radius, int r, int g, int b, double depth_offset=0.0) {
        const double depth = q.depth + depth_offset;
        for (int dy=-radius; dy<=radius; ++dy) for (int dx=-radius; dx<=radius; ++dx) {
            if (dx*dx + dy*dy > radius*radius) continue;
            const int x = static_cast<int>(q.x) + dx;
            const int y = static_cast<int>(q.y) + dy;
            if (x<0||x>=w_||y<0||y>=h_) continue;
            const size_t id = static_cast<size_t>(y*w_+x);
            if (depth > zbuffer_[id]) { zbuffer_[id] = depth; img.set_pixel(x,y,r,g,b); }
        }
    }
    void draw_triangle(Image& img, const Triangle& tri, double angle) {
        auto a = project(tri.a, angle); auto b = project(tri.b, angle); auto c = project(tri.c, angle);
        const int minx = std::max(0, static_cast<int>(std::floor(std::min({a.x,b.x,c.x}))));
        const int maxx = std::min(w_-1, static_cast<int>(std::ceil(std::max({a.x,b.x,c.x}))));
        const int miny = std::max(0, static_cast<int>(std::floor(std::min({a.y,b.y,c.y}))));
        const int maxy = std::min(h_-1, static_cast<int>(std::ceil(std::max({a.y,b.y,c.y}))));
        const double denom = (b.y-c.y)*(a.x-c.x)+(c.x-b.x)*(a.y-c.y);
        if (std::abs(denom) < 1e-12) return;
        Vec3 light = normalize({-0.5, -0.6, 1.0});
        const double shade = std::clamp(0.74 + 0.24*std::abs(dot(normalize(tri.normal), light)), 0.55, 0.98);
        const double pv = std::clamp(tri.potential, 0.0, 1.0);
        int base = static_cast<int>((208 + 18*pv) * shade);
        int r = base, g = base, bl = static_cast<int>((base + 6) * shade);
        for (int y=miny; y<=maxy; ++y) for (int x=minx; x<=maxx; ++x) {
            const double px = x + 0.5, py = y + 0.5;
            const double w1 = ((b.y-c.y)*(px-c.x)+(c.x-b.x)*(py-c.y))/denom;
            const double w2 = ((c.y-a.y)*(px-c.x)+(a.x-c.x)*(py-c.y))/denom;
            const double w3 = 1.0-w1-w2;
            if (w1 < 0 || w2 < 0 || w3 < 0) continue;
            const double depth = w1*a.depth + w2*b.depth + w3*c.depth;
            const size_t id = static_cast<size_t>(y*w_+x);
            if (depth > zbuffer_[id]) {
                zbuffer_[id] = depth;
                img.blend_pixel(x,y,r,g,bl,0.40);
            }
        }
    }
private:
    int w_, h_;
    std::vector<double> zbuffer_;
};

static void local_frame(const Vec3& dir, Vec3& e1, Vec3& e2) {
    Vec3 ref = std::abs(dot(dir, {0,1,0})) > 0.95 ? Vec3{1,0,0} : Vec3{0,1,0};
    e1 = normalize(cross(dir, ref));
    e2 = normalize(cross(dir, e1));
}

static Vec3 ring_point(const Segment& seg, double t, double theta, double radial_scale=1.0) {
    Vec3 dir = normalize(seg.b - seg.a); Vec3 e1,e2; local_frame(dir,e1,e2);
    Vec3 center = lerp(seg.a, seg.b, t);
    double r = seg.radius * (0.97 + 0.04*std::sin(2*PI*t));
    return center + e1*(std::cos(theta)*r*radial_scale) + e2*(std::sin(theta)*r*radial_scale);
}

static std::vector<Triangle> build_tube_mesh(const std::vector<Segment>& segs, int n_t=84, int n_theta=42) {
    std::vector<Triangle> tris;
    for (const auto& seg : segs) {
        for (int it=0; it<n_t; ++it) {
            double t0 = static_cast<double>(it)/n_t;
            double t1 = static_cast<double>(it+1)/n_t;
            for (int ia=0; ia<n_theta; ++ia) {
                double th0 = 2*PI*ia/n_theta;
                double th1 = 2*PI*(ia+1)/n_theta;
                Vec3 p00 = ring_point(seg,t0,th0);
                Vec3 p01 = ring_point(seg,t0,th1);
                Vec3 p10 = ring_point(seg,t1,th0);
                Vec3 p11 = ring_point(seg,t1,th1);
                double pot = 1.0 - (0.5*(p00.y+p11.y)+0.88)/1.62;
                Triangle a{p00,p10,p11, normalize(cross(p10-p00, p11-p00)), pot};
                Triangle b{p00,p11,p01, normalize(cross(p11-p00, p01-p00)), pot};
                tris.push_back(a); tris.push_back(b);
            }
        }
    }
    return tris;
}

static Vec3 conduit_point(const std::vector<Segment>& segs, int branch, double s, double theta, double radial_scale) {
    const Segment& trunk = segs[0];
    const Segment& seg = branch == 0 ? segs[1] : segs[2];
    if (s < 0.52) return ring_point(trunk, s/0.52, theta, radial_scale);
    return ring_point(seg, (s-0.52)/0.48, theta, radial_scale);
}

static std::array<int,3> plasma_like(double v) {
    v = std::clamp(v, 0.0, 1.0);
    int r = static_cast<int>(55 + 210*v);
    int g = static_cast<int>(20 + 205*std::pow(v,1.38));
    int b = static_cast<int>(145 - 115*v);
    return {std::clamp(r,0,255), std::clamp(g,0,255), std::clamp(b,0,255)};
}

static void write_vtk_surface(const std::string& filename, const std::vector<Triangle>& tris) {
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("Could not write VTK file.");
    out << "# vtk DataFile Version 3.0\n";
    out << "Potential-driven biological flow v1.2 surface\n";
    out << "ASCII\nDATASET POLYDATA\n";
    out << "POINTS " << tris.size()*3 << " float\n";
    for (const auto& t : tris) {
        out << t.a.x << " " << t.a.y << " " << t.a.z << "\n";
        out << t.b.x << " " << t.b.y << " " << t.b.z << "\n";
        out << t.c.x << " " << t.c.y << " " << t.c.z << "\n";
    }
    out << "POLYGONS " << tris.size() << " " << tris.size()*4 << "\n";
    for (size_t i=0; i<tris.size(); ++i) out << "3 " << 3*i << " " << 3*i+1 << " " << 3*i+2 << "\n";
    out << "CELL_DATA " << tris.size() << "\nSCALARS potential float 1\nLOOKUP_TABLE default\n";
    for (const auto& t : tris) out << t.potential << "\n";
}

static std::string frame_path(const std::string& outdir, int id) {
    std::ostringstream ss;
    ss << outdir << "/frames/frame_" << std::setw(4) << std::setfill('0') << id << ".ppm";
    return ss.str();
}

int main(int argc, char** argv) {
    const std::string outdir = argc > 1 ? argv[1] : "output";
    std::filesystem::create_directories(outdir + "/frames");
    std::filesystem::create_directories(outdir + "/vtk");
    for (const auto& e : std::filesystem::directory_iterator(outdir + "/frames")) {
        if (e.path().extension() == ".ppm") std::filesystem::remove(e.path());
    }

    const std::vector<Segment> segments = {
        {{0.0, -0.84, 0.0}, {0.0,  0.04, 0.0}, 0.145},
        {{0.0,  0.04, 0.0}, {-0.55, 0.74, 0.10}, 0.086},
        {{0.0,  0.04, 0.0}, {0.58,  0.74,-0.07}, 0.086}
    };
    const auto surface = build_tube_mesh(segments);
    write_vtk_surface(outdir + "/vtk/conduit_surface_v12.vtk", surface);

    const int W = 900, H = 700;
    const int total_frames = 150;
    const int n_particles = 130;
    std::vector<double> phase(n_particles), theta(n_particles), radial(n_particles);
    std::vector<int> branch(n_particles);
    for (int i=0; i<n_particles; ++i) {
        phase[i] = std::fmod(0.011*i + 0.27*std::sin(i*1.19), 1.0); if (phase[i]<0) phase[i]+=1.0;
        theta[i] = 2*PI*std::fmod(0.61803398875*i,1.0);
        radial[i] = 0.10 + 0.72*std::fmod(0.41421356237*i,1.0);
        branch[i] = (i%2==0) ? 0 : 1;
    }

    std::cout << "Potential-driven biological flow demo v1.2\n";
    std::cout << "Generating " << total_frames << " C++ surface-rendered frames...\n";

    for (int f=0; f<total_frames; ++f) {
        Image img(W,H); img.clear(255,255,255);
        Renderer ren(W,H); ren.reset();
        const double rot = 2*PI*f/total_frames;
        const double forcing = 1.0 + 0.18*std::sin(2*PI*f/total_frames);

        draw_text(img, 26, 20, "Potential-driven biological flow demo v1.2", 3, 0,0,0);
        draw_text(img, 26, 62, "u = -K(x) grad P", 3, 0,0,0);
        draw_text(img, 26, 100, "dt c + div(c u) = D lap c + S", 2, 0,0,0);
        draw_text(img, 590, 35, "P(t) =", 2, 0,0,0);
        std::ostringstream pss; pss << std::fixed << std::setprecision(2) << forcing;
        draw_text(img, 680, 35, pss.str(), 2, 0,0,0);

        // Render smooth triangular tube surface.
        for (const auto& tri : surface) ren.draw_triangle(img, tri, rot);

        // Potential-driven transport markers.
        for (int i=0; i<n_particles; ++i) {
            double s = std::fmod(phase[i] + 0.0069*f*forcing, 1.0);
            double th = theta[i] + 0.38*std::sin(0.055*f + 0.21*i);
            Vec3 p = conduit_point(segments, branch[i], s, th, radial[i]);
            Projected q = ren.project(p, rot);
            double v = std::clamp(s,0.0,1.0);
            auto col = plasma_like(v);
            int radius = (s < 0.12 || s > 0.82) ? 4 : 5;
            ren.draw_disk(img, q, radius, col[0], col[1], col[2], 0.13);
        }

        draw_text(img, 292, 276, "outlet / sink", 2, 105,105,105);
        draw_text(img, 625, 276, "outlet / sink", 2, 105,105,105);
        draw_text(img, 470, 550, "inlet forcing", 2, 105,105,105);
        draw_text(img, 5, 665, "C++: geometry + potential field + transport + smooth surface renderer + VTK", 2, 0,0,0);

        img.write_ppm(frame_path(outdir,f));
        if ((f+1)%25==0) std::cout << "  frame " << (f+1) << " / " << total_frames << "\n";
    }
    std::cout << "Done. Frames: " << outdir << "/frames\n";
    std::cout << "VTK surface: " << outdir << "/vtk/conduit_surface_v12.vtk\n";
    return 0;
}
