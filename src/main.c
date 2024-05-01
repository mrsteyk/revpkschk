#include "shared.h"

//~ mrsteyk: @vpk

#define VPKHEADER_MAGIC 0x55AA1234
#define VPKHEADER_VERSION 0x30002

#define VPK_MAX_SIZE_PER_ENTRY 0x100000

#define VPK_ENTRY_TERMINATOR 0xFFFF 

//- mrsteyk: structs
#pragma pack(push, 1)

typedef struct {
    u32 magic;
    union {
        u32 version;
        u16 versions[2];
    };
    
    u32 directory_size;
    u32 embedded_chunk_size;
} VPKHeader;

typedef struct {
    u32 crc;
    // NOTE(mrsteyk): probably embedded size, won't work in Titanfall?
    u16 size;
    u16 fileidx;
} VPKEntryBlock;

typedef struct {
    u32 flags;
    u16 flags2;
    
    u64 offset;
    // NOTE(mrsteyk): compressed_size == decompressed_size ? STORE : LZHAM
    u64 compressed_size;
    u64 decompressed_size;
    
    // NOTE(mrsteyk): non TERMINATOR = continue
    u16 command;
} VPKEntry;

#pragma pack(pop)

//~ mrsteyk: meme

typedef struct VPKFile VPKFile;
struct VPKFile {
    VPKFile* next;
    
    S8 path;
    S8 extension;
    S8 filename;
    HANDLE fh;
    u64 file_size;
    
    VPKEntry* entry_start;
    
    u8 written;
};

static b32
vpkfile_create(Arena* arena, VPKFile* f, S16 file_path) {
    HANDLE fh = CreateFileW(file_path.ptr, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (fh == INVALID_HANDLE_VALUE) {
        return(1);
    }
    f->fh = fh;
    
    LARGE_INTEGER size;
    if (!GetFileSizeEx(fh, &size)) {
        return(2);
    }
    f->file_size = size.QuadPart;
    
    S16 full_filename = S16_strrchr(file_path, L'\\');
    if (full_filename.ptr == 0) {
        f->path = S8_lit(" ");
        
        full_filename = file_path;
    } else {
        // get the inverse string.
        S16 dir16;
        dir16.ptr = file_path.ptr;
        dir16.size = file_path.size - full_filename.size;
        
        full_filename.ptr++;
        full_filename.size--;
        
        S8 dir = str8_from_16(arena, dir16);
        for (u64 i = 0; i < dir.size; i++) {
            if (dir.ptr[i] == '\\')
                dir.ptr[i] = '/';
        }
        f->path = dir;
    }
    
    S16 ext = S16_strrchr(full_filename, L'.');
    // TODO(mrsteyk): .gitignore and alikes
    if (ext.size == full_filename.size) __debugbreak();
    // TODO(mrsteyk): == 1???
    //if (ext.ptr == 0 || ext.size == 1) {
    if (ext.ptr == 0) {
        f->extension = S8_lit(" ");
        
        S16 filename16;
        filename16.ptr = full_filename.ptr;
        filename16.size = full_filename.size - ext.size;
        S8 filename = str8_from_16(arena, filename16);
        f->filename = filename;
    } else {
        S16 ext16 = ext;
        ext16.ptr++;
        ext16.size--;
        S8 ext8 = str8_from_16(arena, ext16);
        f->extension = ext8;
        
        S16 filename16;
        filename16.ptr = full_filename.ptr;
        filename16.size = full_filename.size - ext.size;
        S8 filename = str8_from_16(arena, filename16);
        f->filename = filename;
    }
    
    f->written = 0;
    
    return(0);
}

static b32
vpkfile_create2(Arena* arena, VPKFile* f, S16 dir_path, S16 full_filename, LARGE_INTEGER size, u64 skip_size) {
    HANDLE fh = INVALID_HANDLE_VALUE;
    {
        u64 save = arena->pos;
        u16 path16_size = (dir_path.size + full_filename.size + 1) + 1;
        S16 a;
        a.ptr = (u16*)arena_push_size(arena, path16_size * 2);
        a.size = path16_size - 1;
        memcpy(a.ptr, dir_path.ptr, dir_path.size * 2);
        a.ptr[dir_path.size] = '\\';
        memcpy(a.ptr + dir_path.size + 1, full_filename.ptr, full_filename.size * 2);
        a.ptr[path16_size - 1] = 0;
        
        fh = CreateFileW(a.ptr, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        
        arena_pop_to(arena, save);
    }
    
    if (fh == INVALID_HANDLE_VALUE) {
        return(1);
    }
    
    f->fh = fh;
    f->file_size = size.QuadPart;
    
    if (skip_size) {
        dir_path.ptr += skip_size;
        dir_path.size -= skip_size;
        while(dir_path.size != 0 && dir_path.ptr[0] == L'\\') {
            dir_path.ptr++;
            dir_path.size--;
        }
    }
    
    if (dir_path.ptr == 0 || dir_path.size == 0) {
        f->path = S8_lit(" ");
    } else {
        S8 dir = str8_from_16(arena, dir_path);
        for (u64 i = 0; i < dir.size; i++) {
            if (dir.ptr[i] == '\\')
                dir.ptr[i] = '/';
        }
        f->path = dir;
    }
    
    S16 ext = S16_strrchr(full_filename, L'.');
    // TODO(mrsteyk): .gitignore and alikes
    if (ext.size == full_filename.size) __debugbreak();
    // TODO(mrsteyk): == 1???
    //if (ext.ptr == 0 || ext.size == 1) {
    if (ext.ptr == 0) {
        f->extension = S8_lit(" ");
        
        S16 filename16;
        filename16.ptr = full_filename.ptr;
        filename16.size = full_filename.size - ext.size;
        S8 filename = str8_from_16(arena, filename16);
        f->filename = filename;
    } else {
        S16 ext16 = ext;
        ext16.ptr++;
        ext16.size--;
        S8 ext8 = str8_from_16(arena, ext16);
        //S8 ext8 = ext;
        //ext.ptr++;
        //ext.size--;
        f->extension = ext8;
        
        S16 filename16;
        filename16.ptr = full_filename.ptr;
        filename16.size = full_filename.size - ext.size;
        S8 filename = str8_from_16(arena, filename16);
        f->filename = filename;
    }
    
    f->written = 0;
    
    return(0);
}


static void
vpkfile_write_string(Arena* arena, S8 str) {
    u8* ptr = (u8*)arena_push_size(arena, str.size + 1);
    memcpy(ptr, str.ptr, str.size);
    ptr[str.size] = 0;
}

static void
vpkfile_write_file_entries(Arena* tmp, Arena* arena_dir, Arena* arena_data, VPKFile* f) {
    u64 tmp_pos = tmp->pos;
    
    // TODO(mrsteyk): error checks
    u8* file_data = arena_push_size(tmp, f->file_size);
    ReadFile(f->fh, file_data, f->file_size, 0, 0);
    
    // TODO(mrsteyk): CRC
    VPKEntryBlock* block = (VPKEntryBlock*)arena_push_size(arena_dir, sizeof(*block));
    block->crc = 0;
    block->size = 0;
    block->fileidx = 0x7fff;
    
    u64 num_blocks = (f->file_size + VPK_MAX_SIZE_PER_ENTRY - 1) / VPK_MAX_SIZE_PER_ENTRY;
    u64 file_size_rem = f->file_size;
    for (u64 i = 0; i < num_blocks; i++) {
        VPKEntry* e = (VPKEntry*)arena_push_size(arena_dir, sizeof(*e));
        if (i == 0) {
            f->entry_start = e;
        }
        
        e->flags = 0x101;
        e->flags2 = 0;
        
        u64 chunk_size = VPK_MAX_SIZE_PER_ENTRY;
        if (file_size_rem < VPK_MAX_SIZE_PER_ENTRY) {
            chunk_size = file_size_rem;
        }
        
        // TODO(mrsteyk): LZHAM compress
        e->compressed_size = e->decompressed_size = chunk_size;
        u8* data_ptr = (u8*)arena_push_size(arena_data, chunk_size);
        memcpy(data_ptr, file_data, chunk_size);
        
        e->offset = arena_data->pos - chunk_size - ArenaStartPos(arena_data);
        file_data += chunk_size;
        file_size_rem -= chunk_size;
        
        if (i == (num_blocks - 1)) {
            e->command = VPK_ENTRY_TERMINATOR;
        } else {
            e->command = 0;
        }
    }
    
    f->written = 1;
    
    arena_pop_to(tmp, tmp_pos);
}

//~mrsteyk: @os_walk

#if defined(_WIN32)
static VPKFile*
os_walk_vpk(Arena* arena, S16 dir, VPKFile* h, u32 initial_dir_size) {
    wchar_t* p = (wchar_t*)arena_push_size(arena, (dir.size*2) + sizeof(L"\\*"));
    memcpy(p, dir.ptr, dir.size*2);
    memcpy(p + dir.size, L"\\*", sizeof(L"\\*"));
    
    WIN32_FIND_DATAW ffd;
    HANDLE fh = FindFirstFileW(p, &ffd);
    
    if (fh == INVALID_HANDLE_VALUE) {
        return(0);
    }
    
    //OSWalkFile* prev = 0;
    VPKFile* head = h;
    do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            //_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
            // With current design I couldn't even pop the string out...
            // But it's only couple bytes so who cares...
            S16 fname = S16_from_c(ffd.cFileName);
            if (fname.size == 1 && fname.ptr[0] == L'.')
                continue;
            if (fname.size == 2 && fname.ptr[0] == L'.' && fname.ptr[1] == L'.')
                continue;
            
            u64 new_dir_s = dir.size + fname.size + 1;
            S16 new_dir = {0};
            new_dir.ptr = (u16*)arena_push_size(arena, new_dir_s * 2);
            new_dir.size = new_dir_s;
            memcpy(new_dir.ptr, dir.ptr, dir.size*2);
            new_dir.ptr[dir.size] = L'\\';
            memcpy(new_dir.ptr + dir.size + 1, fname.ptr, fname.size * 2);
            
            head = os_walk_vpk(arena, new_dir, head, initial_dir_size);
        }
        else
        {
            //filesize.LowPart = ffd.nFileSizeLow;
            //filesize.HighPart = ffd.nFileSizeHigh;
            //_tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
            
            LARGE_INTEGER size;
            size.LowPart = ffd.nFileSizeLow;
            size.HighPart = ffd.nFileSizeHigh;
            //VPKFile f;
            
            /*
            //S16 path16 = S16_from_c(ffd.cFileName);
            u64 cfn_l = wcslen(ffd.cFileName);
            u64 path16_size = dir.size + cfn_l + 1;
            S16 path16;
            path16.ptr = (u16*)arena_push_size(arena, path16_size * 2);
            path16.size = path16_size;
            memcpy(path16.ptr, dir.ptr, dir.size * 2);
            path16.ptr[dir.size] = L'\\';
            memcpy(path16.ptr + dir.size + 1, ffd.cFileName, cfn_l * 2);
            */
            
            u64 save = arena->pos;
            VPKFile* f = (VPKFile*)arena_push_size(arena, sizeof(VPKFile));
            memset(f, 0, sizeof(*f));
            S16 fname = S16_from_c(ffd.cFileName);
            b32 ret = vpkfile_create2(arena, f, dir, fname, size, initial_dir_size);
            //S8 path8 = str8_from_16(arena, path16);
            //printf("%.*s %d\n", (int)path8.size, path8.ptr, ret);
            if (ret) {
                S8 dir8 = str8_from_16(arena, dir);
                S8 fname8 = str8_from_16(arena, fname);
                printf("Error %d for `%.*s\\%.*s` %lu\n", ret, (int)dir8.size, dir8.ptr, (int)fname8.size, fname8.ptr, GetLastError());
                arena_pop_to(arena, save);
            } else {
                f->next = head;
                head = f;
            }
        }
    }
    while (FindNextFileW(fh, &ffd) != 0);
    
    return(head);
}
#else
#error No os_walk implemented for this architecture!
#endif

//~ mrsteyk: @main

int
main() {
#if defined(_WIN32)
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    SetConsoleOutputCP(65001);
#else
    LARGE_INTEGER freq = {0};
#endif
    Arena* temp = arena_create(ARENA_DEFAULT_COMMIT, GB(64), ARENA_DEFAULT_ALIGN);
    
    // NOTE(mrsteyk): align must be one to not fuck up the memory layout...
    Arena* arena_dir = arena_create(ARENA_DEFAULT_COMMIT, GB(1), 1);
    Arena* arena_data = arena_create(ARENA_DEFAULT_COMMIT, GB(64), 1);
    
    VPKHeader* header = (VPKHeader*)arena_push_size(arena_dir, sizeof(VPKHeader));
    header->magic = VPKHEADER_MAGIC;
    header->version = VPKHEADER_VERSION;
    
    /*
    S16 path = S16_lit(L"src\\main.c");
    VPKFile f = {0};
    vpkfile_create(temp, &f, path);
    {
        printf("Path %llu: %.*s\n", f.path.size, (int)f.path.size, f.path.ptr);
        printf("Filename %llu: %.*s\n", f.filename.size, (int)f.filename.size, f.filename.ptr);
        printf("Ext %llu: %.*s\n", f.extension.size, (int)f.extension.size, f.extension.ptr);
        printf("File size: %llu\n", f.file_size);
    }
*/
    
    Measurement msr_w[3];
    
    S16 init_dir = S16_lit(L"src");
    perf_measure(&msr_w[0].start);
    VPKFile* h = os_walk_vpk(temp, init_dir, 0, init_dir.size);
    perf_measure(&msr_w[0].end);
    {
        for (VPKFile* c = h; c; c = c->next) {
            printf("-\n");
            printf("Path %llu: `%.*s`\n", c->path.size, (int)c->path.size, c->path.ptr);
            printf("Filename %llu: `%.*s`\n", c->filename.size, (int)c->filename.size, c->filename.ptr);
            printf("Ext %llu: `%.*s`\n", c->extension.size, (int)c->extension.size, c->extension.ptr);
            printf("File size: %llu\n", c->file_size);
        }
        printf("-\n");
    }
    
    perf_measure(&msr_w[1].start);
    //u64 files_written;
    while(1) {
        //files_written = 0;
        
        S8 curr_ext = {0};
        
        for (VPKFile* c = h; c; c = c->next) {
            if (c->written)
                continue;
            curr_ext = c->extension;
            break;
        }
        
        if (curr_ext.size == 0)
            break;
        
        vpkfile_write_string(arena_dir, curr_ext);
        
        while(1) {
            //files_written = 0;
            
            S8 curr_path = {0};
            
            for (VPKFile* c = h; c; c = c->next) {
                if (c->written)
                    continue;
                if (S8_cmp(c->extension, curr_ext))
                    continue;
                
                curr_path = c->path;
                break;
            }
            
            if (curr_path.size == 0)
                break;
            
            vpkfile_write_string(arena_dir, curr_path);
            
            for (VPKFile* c = h; c; c = c->next) {
                if (S8_cmp(c->extension, curr_ext))
                    continue;
                if (S8_cmp(c->path, curr_path))
                    continue;
                
                vpkfile_write_string(arena_dir, c->filename);
                vpkfile_write_file_entries(temp, arena_dir, arena_data, c);
                //files_written++;
            }
            
            // exit out of current path
            vpkfile_write_string(arena_dir, S8_lit(""));
        }
        
        vpkfile_write_string(arena_dir, S8_lit(""));
    }
    
    /*
    vpkfile_write_string(arena_dir, f.extension);
    vpkfile_write_string(arena_dir, f.path);
    vpkfile_write_string(arena_dir, f.filename);
    vpkfile_write_file_entries(temp, arena_dir, arena_data, &f);
    */
    
    // NOTE(mrsteyk): end
    //vpkfile_write_string(arena_dir, S8_lit(""));
    //vpkfile_write_string(arena_dir, S8_lit(""));
    vpkfile_write_string(arena_dir, S8_lit(""));
    
    u64 dir_size = arena_dir->pos - ArenaStartPos(arena_dir);
    header->directory_size = dir_size - sizeof(VPKHeader);
    
    // NOTE(mrsteyk): fixing the offset now
    {
        /*
        u64 num_blocks = (f.file_size + VPK_MAX_SIZE_PER_ENTRY - 1) / VPK_MAX_SIZE_PER_ENTRY;
        for (u64 i = 0; i < num_blocks; i++) {
            f.entry_start[i].offset += dir_size;
        }
*/
        
        for (VPKFile* c = h; c; c = c->next) {
            if (!c->written)
                __debugbreak();
            
            u64 num_blocks = (c->file_size + VPK_MAX_SIZE_PER_ENTRY - 1) / VPK_MAX_SIZE_PER_ENTRY;
            for (u64 i = 0; i < num_blocks; i++) {
                c->entry_start[i].offset += dir_size;
            }
        }
    }
    perf_measure(&msr_w[1].end);
    
    {
        printf("Temp arena pos/commit: %llu %llu\n", temp->pos, temp->commit);
        printf("Header arena pos: %llu\n", arena_dir->pos);
        printf("Header dir size: %u\n", header->directory_size);
        
        printf("Data size: %llu\n", arena_data->pos - ArenaStartPos(arena_data));
    }
    
    perf_measure(&msr_w[2].start);
#if defined(_WIN32)
    HANDLE fh = CreateFileW(L"schk_test_dir.vpk", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    WriteFile(fh, (u8*)arena_dir + ArenaStartPos(arena_dir), dir_size, 0, 0);
    WriteFile(fh, (u8*)arena_data + ArenaStartPos(arena_data), arena_data->pos - ArenaStartPos(arena_data), 0, 0);
#else
#error TODO
#endif
    perf_measure(&msr_w[2].end);
    
    printf("\nWalking directories took: ");
    measurement_print(msr_w[0], freq);
    printf("\nWriting data to memory took: ");
    measurement_print(msr_w[1], freq);
    printf("\nWriting to file took: ");
    measurement_print(msr_w[2], freq);
    printf("\n");
    
    return(0);
}