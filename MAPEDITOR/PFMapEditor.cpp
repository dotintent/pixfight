//
//  PFMapEditor.cpp
//  PixEditor
//
//  Created by Marcin Małysz on 24/04/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include <assert.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#ifndef _WIN32
#include <pwd.h>
#endif

#include "PFMapEditor.hpp"
#include "glTextureLoader.hpp"

/* ===============================================================
 *
 *                          GUI
 *
 * ===============================================================*/
struct icons {
    struct nk_image desktop;
    struct nk_image home;
    struct nk_image computer;
    struct nk_image directory;

    struct nk_image default_file;
    struct nk_image map_file;
};

enum file_groups {
    FILE_GROUP_DEFAULT,
    FILE_GROUP_MAP,
    FILE_GROUP_MAX
};

enum file_types {
    FILE_DEFAULT,
    FILE_MAP,
    FILE_MAX
};

struct file_group {
    enum file_groups group;
    const char *name;
    struct nk_image *icon;
};

struct file {
    enum file_types type;
    const char *suffix;
    enum file_groups group;
};

struct media {
    int font;
    int icon_sheet;
    struct icons icons;
    struct file_group group[FILE_GROUP_MAX];
    struct file files[FILE_MAX];
};

#define MAX_PATH_LEN 512
struct file_browser {
    /* path */
    char file[MAX_PATH_LEN];
    char home[MAX_PATH_LEN];
    char desktop[MAX_PATH_LEN];
    char directory[MAX_PATH_LEN];

    /* directory content */
    char **files;
    char **directories;
    size_t file_count;
    size_t dir_count;
    struct media *media;
};

static void
die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputs("\n", stderr);
    exit(EXIT_FAILURE);
}

static char*
file_load(const char* path, size_t* siz)
{
    char *buf;
    FILE *fd = fopen(path, "rb");
    if (!fd) die("Failed to open file: %s\n", path);
    fseek(fd, 0, SEEK_END);
    *siz = (size_t)ftell(fd);
    fseek(fd, 0, SEEK_SET);
    buf = (char*)calloc(*siz, 1);
    fread(buf, *siz, 1, fd);
    fclose(fd);
    return buf;
}

static char*
str_duplicate(const char *src)
{
    char *ret;
    size_t len = strlen(src);
    if (!len) return 0;
    ret = (char*)malloc(len+1);
    if (!ret) return 0;
    memcpy(ret, src, len);
    ret[len] = '\0';
    return ret;
}

static void
dir_free_list(char **list, size_t size)
{
    size_t i;
    for (i = 0; i < size; ++i)
        free(list[i]);
    free(list);
}

static char**
dir_list(const char *dir, int return_subdirs, size_t *count)
{
    size_t n = 0;
    char buffer[MAX_PATH_LEN];
    char **results = NULL;
    const DIR *none = NULL;
    size_t capacity = 32;
    size_t size;
    DIR *z;

    assert(dir);
    assert(count);
    strncpy(buffer, dir, MAX_PATH_LEN);
    n = strlen(buffer);

    if (n > 0 && (buffer[n-1] != '/'))
        buffer[n++] = '/';

    size = 0;

    z = opendir(dir);
    if (z != none) {
        int nonempty = 1;
        struct dirent *data = readdir(z);
        nonempty = (data != NULL);
        if (!nonempty) return NULL;

        do {
            DIR *y;
            char *p;
            int is_subdir;
            if (data->d_name[0] == '.')
                continue;

            strncpy(buffer + n, data->d_name, MAX_PATH_LEN-n);
            y = opendir(buffer);
            is_subdir = (y != NULL);
            if (y != NULL) closedir(y);

            if ((return_subdirs && is_subdir) || (!is_subdir && !return_subdirs)){
                if (!size) {
                    results = (char**)calloc(sizeof(char*), capacity);
                } else if (size >= capacity) {
                    void *old = results;
                    capacity = capacity * 2;
                    results = (char**)realloc(results, capacity * sizeof(char*));
                    assert(results);
                    if (!results) free(old);
                }
                p = str_duplicate(data->d_name);
                results[size++] = p;
            }
        } while ((data = readdir(z)) != NULL);
    }

    if (z) closedir(z);
    *count = size;
    return results;
}

static struct file_group
FILE_GROUP(enum file_groups group, const char *name, struct nk_image *icon)
{
    struct file_group fg;
    fg.group = group;
    fg.name = name;
    fg.icon = icon;
    return fg;
}

static struct file
FILE_DEF(enum file_types type, const char *suffix, enum file_groups group)
{
    struct file fd;
    fd.type = type;
    fd.suffix = suffix;
    fd.group = group;
    return fd;
}

static struct nk_image*
media_icon_for_file(struct media *media, const char *file)
{
    int i = 0;
    const char *s = file;
    char suffix[4];
    int found = 0;
    memset(suffix, 0, sizeof(suffix));

    /* extract suffix .xxx from file */
    while (*s++ != '\0') {
        if (found && i < 3)
            suffix[i++] = *s;

        if (*s == '.') {
            if (found){
                found = 0;
                break;
            }
            found = 1;
        }
    }

    /* check for all file definition of all groups for fitting suffix*/
    for (i = 0; i < FILE_MAX && found; ++i) {
        struct file *d = &media->files[i];
        {
            const char *f = d->suffix;
            s = suffix;
            while (f && *f && *s && *s == *f) {
                s++; f++;
            }

            /* found correct file definition so */
            if (f && *s == '\0' && *f == '\0')
                return media->group[d->group].icon;
        }
    }
    return &media->icons.default_file;
}

static enum file_groups
media_group_for_file(struct media *media, const char *file)
{
    int i = 0;
    const char *s = file;
    char suffix[4];
    int found = 0;
    memset(suffix, 0, sizeof(suffix));

    /* extract suffix .xxx from file */
    while (*s++ != '\0') {
        if (found && i < 3)
            suffix[i++] = *s;

        if (*s == '.') {
            if (found){
                found = 0;
                break;
            }
            found = 1;
        }
    }

    /* check for all file definition of all groups for fitting suffix*/
    for (i = 0; i < FILE_MAX && found; ++i) {
        struct file *d = &media->files[i];
        {
            const char *f = d->suffix;
            s = suffix;
            while (f && *f && *s && *s == *f) {
                s++; f++;
            }

            /* found correct file definition so */
            if (f && *s == '\0' && *f == '\0')
                return media->group[d->group].group;
        }
    }

    return media->group[FILE_GROUP_DEFAULT].group;
}

static void
media_init(struct media *media)
{
    /* file groups */
    struct icons *icons = &media->icons;
    media->group[FILE_GROUP_DEFAULT] = FILE_GROUP(FILE_GROUP_DEFAULT, "default", &icons->default_file);
    media->group[FILE_GROUP_MAP] = FILE_GROUP(FILE_GROUP_MAP, "map", &icons->map_file);

    /* files */
    media->files[FILE_DEFAULT] = FILE_DEF(FILE_DEFAULT, NULL, FILE_GROUP_DEFAULT);
    media->files[FILE_MAP] = FILE_DEF(FILE_MAP, "map", FILE_GROUP_MAP);
}

static void
file_browser_reload_directory_content(struct file_browser *browser, const char *path)
{
    strncpy(browser->directory, path, MAX_PATH_LEN);
    dir_free_list(browser->files, browser->file_count);
    dir_free_list(browser->directories, browser->dir_count);
    browser->files = dir_list(path, 0, &browser->file_count);
    browser->directories = dir_list(path, 1, &browser->dir_count);
}

static void
file_browser_init(struct file_browser *browser, struct media *media)
{
    memset(browser, 0, sizeof(*browser));
    browser->media = media;
    {
        /* load files and sub-directory list */
        const char *home = getenv("HOME");
#ifdef _WIN32
        if (!home) home = getenv("USERPROFILE");
#else
        if (!home) home = getpwuid(getuid())->pw_dir;
        {
            size_t l;
            strncpy(browser->home, home, MAX_PATH_LEN);
            l = strlen(browser->home);
            strcpy(browser->home + l, "/");
            strcpy(browser->directory, browser->home);
        }
#endif
        {
            size_t l;
            strcpy(browser->desktop, browser->home);
            l = strlen(browser->desktop);
            strcpy(browser->desktop + l, "desktop/");
        }
        browser->files = dir_list(browser->directory, 0, &browser->file_count);
        browser->directories = dir_list(browser->directory, 1, &browser->dir_count);
    }
}

static void
file_browser_free(struct file_browser *browser)
{
    if (browser->files)
        dir_free_list(browser->files, browser->file_count);
    if (browser->directories)
        dir_free_list(browser->directories, browser->dir_count);
    browser->files = NULL;
    browser->directories = NULL;
    memset(browser, 0, sizeof(*browser));
}

static int
file_browser_run(struct file_browser *browser, struct nk_context *ctx)
{
    int ret = 0;
    struct media *media = browser->media;
    struct nk_rect total_space;

    if (nk_begin(ctx, "Load map", nk_rect(50, 50, 800, 500), NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE|NK_WINDOW_TITLE))
    {
        static float ratio[] = {0.25f, NK_UNDEFINED};
        float spacing_x = ctx->style.window.spacing.x;

        /* output path directory selector in the menubar */
        ctx->style.window.spacing.x = 0;
        nk_menubar_begin(ctx);
        {
            char *d = browser->directory;
            char *begin = d + 1;
            nk_layout_row_dynamic(ctx, 25, 6);
            while (*d++) {
                if (*d == '/') {
                    *d = '\0';
                    if (nk_button_label(ctx, begin)) {
                        *d++ = '/'; *d = '\0';
                        file_browser_reload_directory_content(browser, browser->directory);
                        break;
                    }
                    *d = '/';
                    begin = d + 1;
                }
            }
        }
        nk_menubar_end(ctx);
        ctx->style.window.spacing.x = spacing_x;

        /* window layout */
        total_space = nk_window_get_content_region(ctx);
        nk_layout_row(ctx, NK_DYNAMIC, total_space.h, 2, ratio);
        nk_group_begin(ctx, "Special", NK_WINDOW_NO_SCROLLBAR);
        {
            struct nk_image home = media->icons.home;
            struct nk_image desktop = media->icons.desktop;
            struct nk_image computer = media->icons.computer;

            nk_layout_row_dynamic(ctx, 40, 1);
            if (nk_button_image_label(ctx, home, "home", NK_TEXT_CENTERED))
                file_browser_reload_directory_content(browser, browser->home);
            if (nk_button_image_label(ctx,desktop,"desktop",NK_TEXT_CENTERED))
                file_browser_reload_directory_content(browser, browser->desktop);
            if (nk_button_image_label(ctx,computer,"computer",NK_TEXT_CENTERED))
                file_browser_reload_directory_content(browser, "/");
            if (nk_button_label(ctx, "Cancel")) { ret = -1; }

            nk_group_end(ctx);
        }

        /* output directory content window */
        nk_group_begin(ctx, "Content", 0);
        {
            int index = -1;
            size_t i = 0, j = 0, k = 0;
            size_t rows = 0, cols = 0;
            size_t count = browser->dir_count + browser->file_count;

            cols = 4;
            rows = count / cols;
            for (i = 0; i <= rows; i += 1) {
                {size_t n = j + cols;
                    nk_layout_row_dynamic(ctx, 135, (int)cols);
                    for (; j < count && j < n; ++j) {
                        /* draw one row of icons */
                        if (j < browser->dir_count) {
                            /* draw and execute directory buttons */
                            if (nk_button_image(ctx,media->icons.directory))
                                index = (int)j;
                        } else {
                            /* draw and execute files buttons */
                            struct nk_image *icon;
                            size_t fileIndex = ((size_t)j - browser->dir_count);
                            icon = media_icon_for_file(media,browser->files[fileIndex]);

                            enum file_groups group = media_group_for_file(media, browser->files[fileIndex]);

                            if (nk_button_image(ctx, *icon) && group == FILE_GROUP_MAP) {
                                strncpy(browser->file, browser->directory, MAX_PATH_LEN);
                                n = strlen(browser->file);
                                strncpy(browser->file + n, browser->files[fileIndex], MAX_PATH_LEN - n);
                                ret = 1;
                            }
                        }
                    }}
                {size_t n = k + cols;
                    nk_layout_row_dynamic(ctx, 20, (int)cols);
                    for (; k < count && k < n; k++) {
                        /* draw one row of labels */
                        if (k < browser->dir_count) {
                            nk_label(ctx, browser->directories[k], NK_TEXT_CENTERED);
                        } else {
                            size_t t = k-browser->dir_count;
                            nk_label(ctx,browser->files[t],NK_TEXT_CENTERED);
                        }
                    }}
            }

            if (index != -1) {
                size_t n = strlen(browser->directory);
                strncpy(browser->directory + n, browser->directories[index], MAX_PATH_LEN - n);
                n = strlen(browser->directory);
                if (n < MAX_PATH_LEN - 1) {
                    browser->directory[n] = '/';
                    browser->directory[n+1] = '\0';
                }
                file_browser_reload_directory_content(browser, browser->directory);
            }
            nk_group_end(ctx);
        }
    }
    nk_end(ctx);
    return ret;
}

static struct file_browser browser;
static struct media media;

PFMapEditor::PFMapEditor(GLFWwindow *win, const std::string &rootpath,  struct nk_context *ctx)
: _win(win)
, _path(rootpath)
, _ctx(ctx)
, _map(nullptr)
, _screenWidth(1280)
, _screenHeight(800)
, _drawingContext(nullptr)
, _scale(1.0)
, _direction({})
, _mouseLocked(false)
, _layoutMode(PFLayoutModeTiles)
, _penMode(PFPenModeNone)
, _selectedTexture(0)
, _selectedLayer(0)
, _layerImages()
, _selectedImage(0)
, _unitNames({})
, _baseNames({})
, _unit(M_INFANTRY)
, _baseTeam(0)
, _units({})
, _bases({})
, _loading(false)
, _saving(false)
, _resizing(false)
, _cleaning(false)
, _success(false)
, _error(false) {

    PLAYERTEAMSELECTED = 1;

    _unitNames = {"Infantry", "Bazooka", "Jeep", "Tank", "Altillery"};
    _baseNames = {"Capture", "Team"};

    memset(mapsize[0], 0, 20);
    memset(mapsize[1], 0, 20);
    memset(mapname, 0, 128);

    strcpy(mapname, "Unnamed");

    media.icons.home = nk_image_id(textureLoader.loadFile(_path + "home.png", GL_NEAREST, 0, GL_CLAMP_TO_EDGE, true));
    media.icons.directory = nk_image_id(textureLoader.loadFile(_path + "directory.png", GL_NEAREST, 0, GL_CLAMP_TO_EDGE, true));
    media.icons.computer = nk_image_id(textureLoader.loadFile(_path + "computer.png", GL_NEAREST, 0, GL_CLAMP_TO_EDGE, true));
    media.icons.desktop = nk_image_id(textureLoader.loadFile(_path + "desktop.png", GL_NEAREST, 0, GL_CLAMP_TO_EDGE, true));
    media.icons.default_file = nk_image_id(textureLoader.loadFile(_path + "default.png", GL_NEAREST, 0, GL_CLAMP_TO_EDGE, true));
    media.icons.map_file = nk_image_id(textureLoader.loadFile(_path + "map.png", GL_NEAREST, 0, GL_CLAMP_TO_EDGE, true));

    media_init(&media);

    file_browser_init(&browser, &media);
}

PFMapEditor::~PFMapEditor() {

    file_browser_free(&browser);
    release();
}

void PFMapEditor::release() {

    if (_drawingContext) {

        delete _drawingContext;
        _drawingContext = nullptr;
    }

    if (_map) {

        delete _map;
        _map = nullptr;
    }

    for (auto it : _units) {

        delete it;
    }

    for (auto it : _bases) {

        delete it;
    }

    _units.clear();
    _bases.clear();
}

bool PFMapEditor::loadBlank() {

    GAME_IDCOUNTER = 0;

    release();

    glfwGetWindowSize(_win, &_screenWidth, &_screenHeight);

    auto shaderPath = _path + "tile";
    _drawingContext = new DrawingContext(shaderPath, _screenWidth, _screenHeight);
    _drawingContext->calculateFrustum();

    _map = new GameMap(_path);

    int rx = ((int)(_screenWidth / 32.0))+2;
    int ry = ((int)(_screenHeight / 72.0))-2;

    _map->setup(60, 20, rx, ry);

    //textures

    cFGLT *groundTex = new cFGLT();
    std::string groundPath = _path + "tiles.png";
    groundTex->iTexture  = textureLoader.loadFile(groundPath, GL_LINEAR, 0, GL_REPEAT, true);
    groundTex->Width  = 256;
    groundTex->Height =  512;
    groundTex->TileWidth = 64;
    groundTex->TileHeight = 64;
    groundTex->tiles_x = 4;
    groundTex->tiles_y = 8;
    groundTex->active = true;

    _map->textureForLayer(0, groundTex);

    cFGLT *treeTex = new cFGLT();
    std::string treePath = _path + "trees.png";
    treeTex->iTexture  = textureLoader.loadFile(treePath, GL_LINEAR, 0, GL_REPEAT, true);
    treeTex->Width  = 256;
    treeTex->Height =  512;
    treeTex->TileWidth = 64;
    treeTex->TileHeight = 64;
    treeTex->tiles_x = 4;
    treeTex->tiles_y = 8;
    treeTex->active = true;

    _map->textureForLayer(1, treeTex);

    _map->fitTextureForLayer(0);
    _map->fitTextureForLayer(1);

    //////////

    xVec2 size = _map->getMapSize();

    float maxS = 1.0;

    std::cout << "[INFO] Render offset: (" << rx <<"," << ry << ")" << std::endl;

    _map->changeRenderOffset(rx, ry);
    _map->allowScaling(true);

    if (size.x < rx) {

        maxS = (rx / size.x);

        _map->setMinMaxScale(1.0, maxS);
        _map->setScale(maxS);
        _map->allowScaling(false);
    }
    else {

        _map->setScale(maxS);
        _map->setMinMaxScale(0.75, maxS);
    }

    makeLayerImages();

    strcpy(mapname, "Unnamed");

    return true;
}

bool PFMapEditor::load(const std::string &path) {

    GAME_IDCOUNTER = 0;

    for (auto it : _units) {

        delete it;
    }

    for (auto it : _bases) {

        delete it;
    }

    _units.clear();
    _bases.clear();

    std::string mappath = path + ".map";

    if (!_map->load(mappath)) {

        return false;
    }

    xVec2 size = _map->getMapSize();

    int rx = ((int)(_screenWidth / 32.0))+2;
    int ry = ((int)(_screenHeight / 72.0))-2;

    _map->allowScaling(true);
    _map->changeRenderOffset(rx, ry);

    float maxS = 1.0;

    if (size.x < rx) {

        maxS = (rx / size.x);

        _map->setMinMaxScale(1.0, maxS);
        _map->setScale(maxS);
        _map->allowScaling(false);
    }
    else {

        _map->setScale(maxS);
        _map->setMinMaxScale(0.75, maxS);
    }

    _map->move(xVec2(0,0));

    std::string logicpath = path + ".lgf";

    if (!loadLogic(logicpath)) {

        return false;
    }

    return true;
}

bool PFMapEditor::loadLogic(const std::string &path) {

    std::fstream m_file;

    m_file.open(path.c_str(), std::fstream::in|std::fstream::binary);

    if (!m_file) {

        std::cerr << "Error Durning loading file: " << path << " File not found or data corrupte" << std::endl;

        m_file.close();

        return false;
    }

    int32_t teamID = 0;
    int32_t unitID = 0;

    xVec2 pos = xVec2(0,0);
    UNITTYPE type;

    int32_t unitsSize = 0;
    int32_t basesSize = 0;

    GameUnit *newUnit = nullptr;
    GameBase *newBase = nullptr;

    m_file.read((char*)&unitsSize, sizeof(int32_t));

    for (int i = 0; i < unitsSize; ++i) {

        m_file.read((char*)&teamID, sizeof(int32_t));
        m_file.read((char*)&pos, sizeof(xVec2));
        m_file.read((char*)&unitID, sizeof(int32_t));

        type = (UNITTYPE)unitID;

        switch (type) {

            case M_INFANTRY: {

                newUnit = new GameUnit(_path,
                                       "infantry",
                                       "infantryatt",
                                       pos,
                                       xVec2(512,512),
                                       xVec2(64,64),
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(0,0),
                                       teamID);

                newUnit->setMap(_map);
                newUnit->setSize(10);
                newUnit->setUnitType(M_INFANTRY);
                newUnit->setUniqueID(GAME_IDCOUNTER++);

                _units.push_back(newUnit);
            }
                break;

            case M_BAZOOKA: {

                newUnit = new GameUnit(_path,
                                       "bazooka",
                                       "bazookaatt",
                                       pos,
                                       xVec2(512,512),
                                       xVec2(64,64),
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(0,0),
                                       teamID);

                newUnit->setMap(_map);
                newUnit->setSize(10);
                newUnit->setUnitType(M_BAZOOKA);
                newUnit->setUniqueID(GAME_IDCOUNTER++);

                _units.push_back(newUnit);
            }
                break;

            case M_JEEP: {

                newUnit = new GameUnit(_path,
                                       "jeep",
                                       "jeepturret",
                                       pos,
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(0,0),
                                       teamID);

                newUnit->setMap(_map);
                newUnit->setSize(10);
                newUnit->setUnitType(M_JEEP);
                newUnit->setUniqueID(GAME_IDCOUNTER++);

                _units.push_back(newUnit);
            }
                break;

            case M_LTANK: {

                newUnit = new GameUnit(_path,
                                       "tank",
                                       "turret",
                                       pos,
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(512,512),
                                       xVec2(80,80),
                                       xVec2(-6,-6),
                                       teamID);

                newUnit->setMap(_map);
                newUnit->setSize(10);
                newUnit->setUnitType(M_LTANK);
                newUnit->setUniqueID(GAME_IDCOUNTER++);

                _units.push_back(newUnit);
            }
                break;

            case M_ARTILLERY: {

                newUnit = new GameUnit(_path,
                                       "tank",
                                       "artilleryturret",
                                       pos,
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(512,512),
                                       xVec2(64,64),
                                       xVec2(4,-2),
                                       teamID);

                newUnit->setMap(_map);
                newUnit->setSize(10);
                newUnit->setUnitType(M_ARTILLERY);
                newUnit->setUniqueID(GAME_IDCOUNTER++);

                _units.push_back(newUnit);
            }
                break;

            default:
                break;
        }

    }

    m_file.read((char*)&basesSize, sizeof(int32_t));

    for(int ib = 0; ib < basesSize; ++ib) {

        m_file.read((char*)&teamID, sizeof(int32_t));
        m_file.read((char*)&pos, sizeof(xVec2));

        newBase = new GameBase(_path,
                               xVec2(512.0,64.0),
                               xVec2(64.0,64.0),
                               teamID);

        newBase->setMap(_map);
        newBase->setPosition(pos);
        newBase->setUniqueID(GAME_IDCOUNTER++);

        _bases.push_back(newBase);
    }

    m_file.close();

    return true;
}

bool PFMapEditor::save(const std::string &path) {

    std::string mappath = path + ".map";

    if (!_map->save(mappath)) {

        return false;
    }

    std::string logicpath = path + ".lgf";

    if (!saveLogic(logicpath)) {

        return false;
    }

    return true;
}

bool PFMapEditor::saveLogic(const std::string &path) {

    std::fstream m_file;

    m_file.open(path.c_str(), std::fstream::out|std::fstream::binary);

    if (!m_file) {

        std::cerr << "Error Durning saving file: " << path << " probably no disk space or access denied." << std::endl;

        m_file.close();

        return false;
    }

    int32_t teamID = 0;
    int32_t unitID = 0;

    xVec2 pos = xVec2(0,0);

    int32_t unitsSize = _units.size();
    int32_t basesSize = _bases.size();

    m_file.write((const char*)&unitsSize, sizeof(int32_t));

    for (auto unit : _units) {

        teamID = unit->getTeamID();
        pos = unit->getRealPosition();
        unitID = unit->getUnitType();

        m_file.write((const char*)&teamID, sizeof(int32_t));
        m_file.write((const char*)&pos, sizeof(xVec2));
        m_file.write((const char*)&unitID, sizeof(int32_t));
    }

    m_file.write((const char*)&basesSize, sizeof(int32_t));

    for(auto base : _bases) {

        teamID = base->getTeamID();
        pos = base->getPosition();

        m_file.write((const char*)&teamID, sizeof(int32_t));
        m_file.write((const char*)&pos, sizeof(xVec2));
    }

    m_file.close();

    return true;
}

void PFMapEditor::handleScroll(double xoff, double yoff) {

    if (_mouseLocked) {
        return;
    }
    _scale = _map->getCurrendScale();
    _scale -= yoff * 0.05;
    _map->setScale(_scale);
}

void PFMapEditor::handleMouse(double x, double y, int button, int action) {

    static xVec2 initialPos;
    static bool dragging;

    if (action == 1 && !dragging) {

        dragging = true;
        initialPos = xVec2(x, y);
    }
    else if (action == 0 && dragging) {

        dragging = false;
        initialPos = xVec2(0, 0);
    }

    if (!dragging || _mouseLocked) {
        return;
    }

    if (button == 0) {

        _map->testPoint(xVec2(x, y));

        switch (_penMode) {

            case PFPenModeDrawing: {

                int tx = (int)(_selectedImage % 4);
                int ty = (int)(_selectedImage / 4);

                _map->setTileForLayer(_selectedLayer, xVec2(tx, ty));
            }
                break;
            case PFPenModeLock: {

                _map->lockHex();
            }
                break;
            case PFPenModeUnlock: {

                _map->unlockHex();
            }
                break;
            case PFPenModeClear: {

                _map->clearTile(_selectedLayer);
            }
                break;
            case PFPenModeDeleteObj: {

                dragging = false;

                auto hitpoint = _map->hitTest();

                for (auto it = _units.begin(); it != _units.end(); ) {

                    auto pos = (*it)->getRealPosition();

                    if (AlmostEqual(pos, hitpoint)) {

                        delete *it;
                        it = _units.erase(it);
                    }
                    else {

                        ++it;
                    }
                }

                for (auto it = _bases.begin(); it != _bases.end(); ) {

                    if ((*it)->isSelected()) {

                        delete *it;
                        it = _bases.erase(it);
                    }
                    else {

                        ++it;
                    }
                }
            }
                break;
            case PFPenModeAddUnit: {

                dragging = false;
                addUnit(_map->hitTest());
            }
                break;
            case PFPenModeAddBase: {

                dragging = false;
                addBase(_map->hitTest());
            }
                break;
            case PFPenModeNone:
                break;
        }
    }
    else if (button == 1) {

        _direction = _map->getBias();
        auto pos = xVec2(x, y);
        auto dir = (pos - initialPos);
        initialPos = pos;
        _direction -= dir;
    }
}

void PFMapEditor::Render() {

    if (!_map) {
        return;
    }

    _map->move(_direction);

    auto tileShader = _drawingContext->getTileShader();
    tileShader->begin();

    _map->draw(*_drawingContext);

    for (auto base : _bases) {

        base->setScale(_map->getCurrendScale());
        base->draw(*_drawingContext);
    }

    for (auto unit : _units) {

        unit->setScale(_map->getCurrendScale());
        unit->draw(*_drawingContext);
    }

    _drawingContext->unbindVertexArray();

    renderInterface();
}

void PFMapEditor::renderInterface() {

    static struct nk_style_button button_style = _ctx->style.button;
    button_style.normal = nk_style_item_color(nk_rgba(245, 184, 48, 255));
    button_style.hover  = nk_style_item_color(nk_rgba(245, 184, 48, 255));

    if (!_cleaning && !_loading && !_saving && !_resizing) {

        if (nk_begin(_ctx, "MENU", nk_rect(0, 0, 200, 190), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR)) {

            nk_layout_row_static(_ctx, 32, 180, 1);
            if (nk_button_label(_ctx, "NEW MAP")) {

                _cleaning = true;
                _penMode = PFPenModeNone;
            }

            nk_layout_row_static(_ctx, 32, 180, 1);
            if (nk_button_label(_ctx, "RESIZE")) {

                xVec2 size = _map->getMapSize();

                strcpy(mapsize[0], std::to_string((int)size.x).c_str());
                strcpy(mapsize[1], std::to_string((int)size.y).c_str());

                _resizing = true;
                _penMode = PFPenModeNone;
            }

            nk_layout_row_static(_ctx, 32, 180, 1);
            if (nk_button_label(_ctx, "LOAD")) {

                _loading = true;
                _penMode = PFPenModeNone;
            }

            nk_layout_row_static(_ctx, 32, 180, 1);
            if (nk_button_label(_ctx, "SAVE")) {

                _saving = true;
                _penMode = PFPenModeNone;
            }

        }
        nk_end(_ctx);

        if (nk_begin(_ctx, "TOOLS", nk_rect(_screenWidth-320, 0, 320, 600), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE )) {

            nk_layout_row_dynamic(_ctx, 30, 2);
            if (nk_option_label(_ctx, "Tiles", _layoutMode == PFLayoutModeTiles)) { _layoutMode = PFLayoutModeTiles; }
            if (nk_option_label(_ctx, "Units", _layoutMode == PFLayoutModeUnits)) { _layoutMode = PFLayoutModeUnits; }

            switch (_layoutMode) {

                case PFLayoutModeTiles: {

                    nk_layout_row_static(_ctx, 284, 294, 1);

                    //GRID
                    if (nk_group_begin(_ctx, "Tiles", NK_WINDOW_BORDER)) {

                        nk_layout_row_static(_ctx, 64, 64, 4);

                        for (int i = 0; i < MaxTilesCount; ++i) {

                            if ((_selectedImage == i) ? nk_button_image_styled(_ctx, &button_style, _layerImages[i]) : nk_button_image(_ctx, _layerImages[i])) {

                                _selectedImage = i;
                                _penMode = PFPenModeDrawing;
                            }
                        }

                    }
                    nk_group_end(_ctx);

                    nk_layout_row_static(_ctx, 40, 294, 1);

                    //combo
                    if (nk_combo_begin_label(_ctx, _selectedTexture ? "Trees" : "Tiles", nk_vec2(nk_widget_width(_ctx), 100))) {
                        nk_layout_row_dynamic(_ctx, 25, 1);

                        if (nk_combo_item_label(_ctx, "Tiles", NK_TEXT_LEFT)) {
                            _selectedTexture = 0;
                            makeLayerImages();
                        }

                        if (nk_combo_item_label(_ctx, "Trees", NK_TEXT_LEFT)) {
                            _selectedTexture = 1;
                            makeLayerImages();
                        }

                        nk_combo_end(_ctx);
                    }

                    nk_layout_row_static(_ctx, 40, 100, 2);

                    if (nk_option_label(_ctx, "BOTTOM", _selectedLayer == 0)) { _selectedLayer = 0; }
                    if (nk_option_label(_ctx, "TOP", _selectedLayer == 1)) { _selectedLayer = 1; }

                    nk_layout_row_static(_ctx, 30, 290, 1);

                    if (nk_button_label(_ctx, "Rubber")) {

                        _penMode = PFPenModeClear;
                    }

                    nk_layout_row_static(_ctx, 30, 94, 3);

                    if (nk_button_label(_ctx, "Lock HEX")) {

                        _penMode = PFPenModeLock;
                    }

                    if (nk_button_label(_ctx, "Unlock HEX")) {

                        _penMode = PFPenModeUnlock;
                    }

                    if (nk_button_label(_ctx, "Hide/UnHide")) {

                        _map->hideLock = !_map->hideLock;
                    }

                    nk_layout_row_dynamic(_ctx, 30, 1);
                    nk_label(_ctx, "Pen size:", NK_TEXT_ALIGN_LEFT);

                    nk_layout_row_dynamic(_ctx, 30, 4);

                    if (nk_option_label(_ctx, "Pen 1", _map->penSize == 0)) { _map->penSize = 0; }
                    if (nk_option_label(_ctx, "Pen 2", _map->penSize == 1)) { _map->penSize = 1; }
                    if (nk_option_label(_ctx, "Pen 3", _map->penSize == 2)) { _map->penSize = 2; }
                    if (nk_option_label(_ctx, "Pen 4", _map->penSize == 3)) { _map->penSize = 3; }
                }
                    break;

                case PFLayoutModeUnits : {

                    nk_layout_row_dynamic(_ctx, 30, 4);

                    if (nk_option_label(_ctx, "Blue", PLAYERTEAMSELECTED == 1)) { PLAYERTEAMSELECTED = 1; }
                    if (nk_option_label(_ctx, "Red", PLAYERTEAMSELECTED == 2)) { PLAYERTEAMSELECTED = 2; }
                    if (nk_option_label(_ctx, "Green", PLAYERTEAMSELECTED == 3)) { PLAYERTEAMSELECTED = 3; }
                    if (nk_option_label(_ctx, "Yellow", PLAYERTEAMSELECTED == 4)) { PLAYERTEAMSELECTED = 4; }

                    nk_layout_row_static(_ctx, 32, 140, 2);

                    //units
                    if (nk_combo_begin_label(_ctx, _unitNames[_unit].c_str(), nk_vec2(nk_widget_width(_ctx), 100))) {
                        nk_layout_row_dynamic(_ctx, 25, 1);

                        for (int u = 0; u < _unitNames.size(); ++u) {

                            if (nk_combo_item_label(_ctx, _unitNames[u].c_str(), NK_TEXT_CENTERED)) {

                                _unit = (UNITTYPE)u;
                            }
                        }

                        nk_combo_end(_ctx);
                    }

                    //bases
                    if (nk_combo_begin_label(_ctx, _baseNames[_baseTeam].c_str(), nk_vec2(nk_widget_width(_ctx), 100))) {
                        nk_layout_row_dynamic(_ctx, 25, 1);

                        for (int u = 0; u < _baseNames.size(); ++u) {

                            if (nk_combo_item_label(_ctx, _baseNames[u].c_str(), NK_TEXT_CENTERED)) {

                                _baseTeam = u;
                            }
                        }

                        nk_combo_end(_ctx);
                    }

                    nk_layout_row_static(_ctx, 40, 284, 1);

                    if (nk_button_label(_ctx, "ADD UNIT")) {

                        _penMode = PFPenModeAddUnit;
                    }

                    if (nk_button_label(_ctx, "ADD BASE")) {

                        _penMode = PFPenModeAddBase;
                    }

                    if (nk_button_label(_ctx, "DELETE")) {

                        _penMode = PFPenModeDeleteObj;
                    }

                    if (nk_button_label(_ctx, "SAVE CAMERA")) {

                        _map->saveCamForPlayer(PLAYERTEAMSELECTED-1);
                    }

                }
                    break;
            }

        }
        nk_end(_ctx);
    }

    if (_cleaning) {

        if (nk_begin(_ctx, "WARNING", nk_rect((_screenWidth - 400) * 0.5, (_screenHeight - 120) * 0.5, 400, 120), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR)) {

            nk_layout_row_dynamic(_ctx, 32, 1);
            nk_label(_ctx, "Are you sure you want to make new map ?", NK_TEXT_ALIGN_CENTERED);

            nk_layout_row_static(_ctx, 40, 180, 2);

            if (nk_button_label(_ctx, "YES")) {

                loadBlank();
                _cleaning = false;
            }

            if (nk_button_label(_ctx, "CANCEL")) {

                _cleaning = false;
            }

        }
        nk_end(_ctx);

    }

    if (_resizing) {

        if (nk_begin(_ctx, "RESIZE", nk_rect((_screenWidth - 400) * 0.5, (_screenHeight - 200) * 0.5, 400, 200), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR)) {

            nk_layout_row_dynamic(_ctx, 32, 1);
            nk_label(_ctx, "Set new map size: ", NK_TEXT_ALIGN_CENTERED);

            nk_layout_row_static(_ctx, 30, 80, 4);

            nk_label(_ctx, "X: ", NK_TEXT_ALIGN_CENTERED);
            nk_edit_string_zero_terminated(_ctx, NK_EDIT_FIELD, mapsize[0], 20, nk_filter_decimal);
            nk_label(_ctx, "Y: ", NK_TEXT_ALIGN_CENTERED);
            nk_edit_string_zero_terminated(_ctx, NK_EDIT_FIELD, mapsize[1], 20, nk_filter_decimal);

            nk_layout_row_static(_ctx, 10, 0, 1);
            nk_spacing(_ctx, 1);

            nk_layout_row_static(_ctx, 40, 180, 2);

            if (nk_button_label(_ctx, "YES")) {

                int nx = std::stoi(mapsize[0]);
                int ny = std::stoi(mapsize[1]);

                _map->resize(nx, ny);

                xVec2 size = _map->getMapSize();
                xVec2 rs = _map->getRenderSize();

                float maxS = 1.0;
                _map->allowScaling(true);

                if (size.x < rs.x) {

                    maxS = (rs.x / size.x);

                    _map->setMinMaxScale(1.0, maxS);
                    _map->setScale(maxS);
                    _map->allowScaling(false);
                }
                else {

                    _map->setScale(maxS);
                    _map->setMinMaxScale(0.75, maxS);
                }

                _resizing = false;
            }

            if (nk_button_label(_ctx, "CANCEL")) {

                _resizing = false;
            }

        }
        nk_end(_ctx);
    }

    if (_loading) {

        int retval = file_browser_run(&browser, _ctx);

        if (retval == -1) {

            _loading = false;
        }

        if (retval == 1) {

            std::string filepath = browser.file;
            std::string loadpath = filepath.substr(0, filepath.size() - 4);

            if (load(loadpath)) {

                _success = true;
            }
            else {

                _error = true;
            }

            _loading = false;
        }
    }

    if (_saving) {

        if (nk_begin(_ctx, "SAVE", nk_rect((_screenWidth - 420) * 0.5, (_screenHeight - 150) * 0.5, 420, 150), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR)) {

            nk_layout_row_dynamic(_ctx, 26, 2);
            nk_label(_ctx, "Enter map name: ", NK_TEXT_ALIGN_CENTERED);

            nk_edit_string_zero_terminated(_ctx, NK_EDIT_FIELD, mapname, 126, nk_filter_ascii);

            int len = strlen(mapname);

            nk_layout_row_dynamic(_ctx, 32, len > 0 ? 3 : 2);
            nk_spacing(_ctx, 1);

            if (len > 0 ) {

                if (nk_button_label(_ctx, "SAVE")) {

                    std::string savepath = _path + "../maps/" + mapname;

                    if (save(savepath)) {

                        _success = true;
                    }
                    else {

                        _error = true;
                    }

                    _saving = false;
                }
            }

            if (nk_button_label(_ctx, "CANCEL")) {

                _saving = false;
            }

            nk_layout_row_dynamic(_ctx, 20, 1);
            nk_label(_ctx, "*If you enter a name of already existing map it will be overwrite!", NK_TEXT_ALIGN_RIGHT);

        }
        nk_end(_ctx);
    }

    //display info
    if (_success || _error) {

        if (nk_begin(_ctx, "INFO", nk_rect((_screenWidth - 200) * 0.5, (_screenHeight - 120) * 0.5, 200, 120), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR)) {

            std::string message = _success ? "Operation successfull" : "Operation fail";

            nk_layout_row_dynamic(_ctx, 50, 1);
            nk_label(_ctx, message.c_str(), NK_TEXT_ALIGN_CENTERED);
            nk_layout_row_dynamic(_ctx, 24, 1);

            if (nk_button_label(_ctx, "OK")) {

                _success = false;
                _error = false;
            }
        }
        nk_end(_ctx);
    }

    _mouseLocked = nk_window_is_any_hovered(_ctx);
}

void PFMapEditor::makeLayerImages() {

    std::string texPath = _path + (_selectedTexture == 0 ? "tiles.png" : "trees.png");
    GLuint tex = textureLoader.loadFile(texPath, GL_LINEAR, 0, GL_REPEAT, true);

    for (int i = 0; i<MaxTilesCount; ++i) {

        int xidx = i % 4;
        int yidx = i / 4;

        _layerImages[i] = nk_subimage_id(tex, 256, 512, nk_rect(xidx * 64, yidx * 64, 64, 64));
    }
}

void PFMapEditor::addUnit(const xVec2 &pos) {

    int teamID = PLAYERTEAMSELECTED;

    GameUnit *newUnit = nullptr;

    switch (_unit) {
        case M_INFANTRY: {

            newUnit = new GameUnit(_path,
                                   "infantry",
                                   "infantryatt",
                                   pos,
                                   xVec2(512,512),
                                   xVec2(64,64),
                                   xVec2(256,512),
                                   xVec2(64,64),
                                   xVec2(0,0),
                                   teamID);

            newUnit->setMap(_map);
            newUnit->setSize(10);
            newUnit->setUnitType(M_INFANTRY);
            newUnit->setUniqueID(GAME_IDCOUNTER++);

            _units.push_back(newUnit);
        }
            break;

        case M_BAZOOKA: {

            newUnit = new GameUnit(_path,
                                   "bazooka",
                                   "bazookaatt",
                                   pos,
                                   xVec2(512,512),
                                   xVec2(64,64),
                                   xVec2(256,512),
                                   xVec2(64,64),
                                   xVec2(0,0),
                                   teamID);

            newUnit->setMap(_map);
            newUnit->setSize(10);
            newUnit->setUnitType(M_BAZOOKA);
            newUnit->setUniqueID(GAME_IDCOUNTER++);

            _units.push_back(newUnit);
        }
            break;

        case M_JEEP: {

            newUnit = new GameUnit(_path,
                                   "jeep",
                                   "jeepturret",
                                   pos,
                                   xVec2(256,512),
                                   xVec2(64,64),
                                   xVec2(256,512),
                                   xVec2(64,64),
                                   xVec2(0,0),
                                   teamID);

            newUnit->setMap(_map);
            newUnit->setSize(10);
            newUnit->setUnitType(M_JEEP);
            newUnit->setUniqueID(GAME_IDCOUNTER++);

            _units.push_back(newUnit);
        }
            break;

        case M_LTANK: {

            newUnit = new GameUnit(_path,
                                   "tank",
                                   "turret",
                                   pos,
                                   xVec2(256,512),
                                   xVec2(64,64),
                                   xVec2(512,512),
                                   xVec2(80,80),
                                   xVec2(-6,-6),
                                   teamID);

            newUnit->setMap(_map);
            newUnit->setSize(10);
            newUnit->setUnitType(M_LTANK);
            newUnit->setUniqueID(GAME_IDCOUNTER++);

            _units.push_back(newUnit);
        }
            break;

        case M_ARTILLERY:{

            newUnit = new GameUnit(_path,
                                   "tank",
                                   "artilleryturret",
                                   pos,
                                   xVec2(256,512),
                                   xVec2(64,64),
                                   xVec2(512,512),
                                   xVec2(64,64),
                                   xVec2(4,-2),
                                   teamID);

            newUnit->setMap(_map);
            newUnit->setSize(10);
            newUnit->setUnitType(M_ARTILLERY);
            newUnit->setUniqueID(GAME_IDCOUNTER++);

            _units.push_back(newUnit);
        }
            break;

        default:
            break;
    }
}

void PFMapEditor::addBase(const xVec2 &pos) {

    GameBase *newBase = new GameBase(_path,
                                     xVec2(512.0,64.0),
                                     xVec2(64.0,64.0),
                                     _baseTeam ? PLAYERTEAMSELECTED : 0);

    newBase->setMap(_map);
    newBase->setPosition(pos);
    newBase->setUniqueID(GAME_IDCOUNTER++);

    _bases.push_back(newBase);
}
