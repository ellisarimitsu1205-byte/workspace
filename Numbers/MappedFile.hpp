#pragma once

#include <cerrno>
#include <cstddef>
#include <filesystem>
#include <span>
#include <stdexcept>
#include <system_error>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>


struct MappedFile
{
    struct MappedFileDetails
    {
        constexpr static int InvalidDescriptor = -1;
        int Descriptor{ InvalidDescriptor };
        char* Begin{ nullptr };
        std::size_t Size{ 0 };


        [[nodiscard]] bool IsOpen() const
        {
            return Descriptor != InvalidDescriptor;
        }

        void Close()
        {
            if(Begin != nullptr)
            {
                munmap(Begin, Size);
                Begin = nullptr;
                Size = 0;
            }
            if(Descriptor == InvalidDescriptor)
            {
                return;
            }

            close(Descriptor);
            Descriptor = InvalidDescriptor;
        }
    };

    MappedFile(const std::filesystem::path& filePath)
    {
        const auto descriptor = open(filePath.c_str(), O_RDONLY);
        if(descriptor == -1)
        {
            throw std::system_error(errno, std::system_category(), "Failed to open file");
        }
        

        struct stat sb; 
        if(fstat(descriptor, &sb) == -1)
        {
            const int error = errno;
            close(descriptor);
            throw std::system_error(error, std::system_category(), "Failed to get file status");
        }
        if(sb.st_size == 0)
        {
            close(descriptor);
            throw std::runtime_error("Cannot map an empty file");
        }

        const auto start = static_cast<char*>(mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, descriptor, 0));
        if(start == MAP_FAILED)
        {
            const int error = errno;
            close(descriptor);
            throw std::system_error(error, std::system_category(), "Failed to map file");
        }

        madvise(start, sb.st_size, MADV_SEQUENTIAL);

        details_.Descriptor = descriptor;
        details_.Begin = start;
        details_.Size = static_cast<std::size_t>(sb.st_size);   
    }

    MappedFile(const MappedFile&) = delete;
    MappedFile& operator=(const MappedFile&) = delete;

    [[nodiscard]] std::span<const char> Data() const {
        return { details_.Begin, details_.Size };
    }

    ~MappedFile()
    {
        details_.Close();
    }
    private:
    MappedFileDetails details_;
};