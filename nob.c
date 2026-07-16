#define NOB_IMPLEMENTATION
#include <stdbool.h>
#include "nob.h"

#define COMPILER "g++"
#define BUILD_FOLDER "build/"
#if defined(_WIN32)
#define OUTPUT_EXEC "build/myCanvas.exe"
#elif defined(__linux__)
#define OUTPUT_EXEC "build/myCanvas"
#endif

Procs procs = {0};

typedef struct {
	const char* sourcePath;
} sourceObject;

typedef struct {
	const char** links;
	size_t count;
} linkerFlags;

typedef struct {
	const char** paths;
	size_t count;
} includePaths;

typedef struct {
	const char* sourcePath;
	const char* buildPath;
	Nob_Cmd cmd;
} buildScript;

typedef struct {
	buildScript* items;
	size_t count;
	size_t capacity;
} buildScripts;

typedef struct {
	const char* buildPath;
} buildObject;

typedef struct {
	buildObject* items;
	size_t count;
	size_t capacity;
} buildObjects;

buildScripts generateBuildScripts(const char* sources[], size_t sourcesCount, includePaths includes) {
	buildScripts objects = {0};

	for(size_t i = 0; i < sourcesCount; ++i) {
		Nob_Cmd cmd = {0};
		nob_cmd_append(&cmd, COMPILER);
		
		const char* sourcePath = sources[i];
		const char* buildPath = nob_temp_sprintf(BUILD_FOLDER"%s.o", nob_path_name(sources[i]));
		nob_cc_inputs(&cmd, sourcePath);
		nob_cmd_append(&cmd, "-c");
		nob_cc_output(&cmd, buildPath);

		for(size_t j = 0; j < includes.count; ++j) {
			nob_cmd_append(&cmd, includes.paths[j]);
		}

		buildScript obj = {0};
		obj.buildPath = buildPath;
		obj.sourcePath = sourcePath;
		obj.cmd = cmd;

		da_append(&objects, obj);
	}

	return objects;
}


buildObjects executeBuildScripts(buildScripts scripts, bool async) {
	buildObjects objects = {0};

	da_foreach(buildScript, script, &scripts) {
		nob_log(NOB_INFO, "Building Object: %s", script->buildPath);
		if(async){
			if(!nob_cmd_run(&script->cmd, .async=&procs)) {
				nob_log(NOB_ERROR, "Failed to build %s", script->buildPath);
				return objects;
			}
		}else{
			if(!nob_cmd_run(&script->cmd)) {
				nob_log(NOB_ERROR, "Failed to build %s", script->buildPath);
				return objects;
			}
		}

		buildObject obj = {0};
		obj.buildPath = script->buildPath;

		da_append(&objects, obj);
	}


	return objects;
}


bool compileObjects(buildObjects objects, linkerFlags links, const char* outputExec) {
	Nob_Cmd cmd = {0};

	if(!objects.count){
		nob_log(WARNING, "No input files for compilation!");
		return false;
	}
	
	nob_cmd_append(&cmd, COMPILER);
	nob_cc_output(&cmd, outputExec);

	da_foreach(buildObject, obj, &objects) {
		nob_cc_inputs(&cmd, obj->buildPath);
	}

	for(size_t i = 0; i < links.count; ++i) {
		nob_cmd_append(&cmd, links.links[i]);
	}

	return nob_cmd_run(&cmd);
}

void json_escape_fputs(const char *s, FILE *f)
{
    for (; *s; ++s) {
        switch (*s) {
            case '"':  fputs("\\\"", f); break;
            case '\\': fputs("\\\\", f); break;
            case '\n': fputs("\\n", f);  break;
            case '\t': fputs("\\t", f);  break;
            default:   fputc(*s, f);
        }
    }
}

void json_escape_path_fputs(const char *s, FILE *f)
{
    for (; *s; ++s) {
        char c = (*s == '\\') ? '/' : *s;
        if (c == '"') fputs("\\\"", f);
        else          fputc(c, f);
    }
}

bool generate_compile_commands(buildScripts scripts)
{
    FILE *f = fopen("build/compile_commands.json", "w");
    if (!f) return false;

    const char *cwd = nob_get_current_dir_temp(); // hoisted, doesn't change per-iteration

    fprintf(f, "[\n");

    for (size_t i = 0; i < scripts.count; ++i) {
        buildScript *script = &scripts.items[i];

        fprintf(f, "  {\n    \"directory\": \"");
        json_escape_fputs(cwd, f);
        fprintf(f, "\",\n    \"file\": \"");
        json_escape_fputs(script->sourcePath, f);
        fprintf(f, "\",\n    \"arguments\": [");

        for (size_t j = 0; j < script->cmd.count; ++j) {
            if (j) fprintf(f, ", ");
            fputc('"', f);
            json_escape_fputs(script->cmd.items[j], f);
            fputc('"', f);
        }

        fprintf(f, "]\n  }%s\n", i + 1 < scripts.count ? "," : "");
    }

    fprintf(f, "]\n");
    fclose(f);
    return true;
}



int main(int argc, char** argv){
	NOB_GO_REBUILD_URSELF(argc, argv);
	if(!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

	bool asnyc = false;

	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-j") == 0) {
			asnyc = true;
		} else {
			nob_log(NOB_ERROR, "Unknown flag: %s", argv[i]);
			return 1;
		}
	}


	const char* sourcesToBuild[] = {
		"src/main.cpp",
		"src/canvas.cpp",
		"src/canvas_misc.cpp",
		"src/canvas_render.cpp",
		"src/canvas_update.cpp",
		"src/helpers.cpp",
		"src/SDLHandler.cpp"
	};

	const char* paths[] = {
		"-Iinclude/",
#if defined(_WIN32)
		"-Ivendor/raylib/windows/include/",
		"-Ivendor/SDL/windows/include/",
#elif defined(__linux__)
		"-Ivendor/raylib/unix/include/",
		"-Ivendor/SDL/unix/include/",
#endif
	};

	includePaths includes = {
		.paths = paths,
		.count = ARRAY_LEN(paths)
	};

	const char* toLink[] =
	{
#if defined(_WIN32)
		"-Lvendor/raylib/windows/lib/", "-lraylib",
		"-Lvendor/SDL/windows/lib/", "-lSDL3",
		"-lm", "-lhid", "-lsetupapi", "-lwinmm", "-limm32",
		"-lversion", "-lole32", "-loleaut32", "-luuid", "-ladvapi32",
		"-luser32", "-lgdi32", "-lshell32",
#elif defined(__linux__)
		"-Lvendor/raylib/unix/lib/", "-l:libraylib.a",
		"-Lvendor/SDL/unix/lib/", "-lSDL3",
		"-lX11",
#endif
	};

	linkerFlags links = {
		.links = toLink,
		.count = ARRAY_LEN(toLink)
	};

	buildScripts scripts = generateBuildScripts(sourcesToBuild, ARRAY_LEN(sourcesToBuild), includes);
	generate_compile_commands(scripts);

	buildObjects objects = executeBuildScripts(scripts, asnyc);

	if(asnyc)
		nob_procs_wait(procs);

	if(compileObjects(objects, links, OUTPUT_EXEC))
		nob_log(NOB_INFO, "Compilation Succesful!");
	else
		nob_log(NOB_ERROR, "Compilation Unsuccesful!");
}
