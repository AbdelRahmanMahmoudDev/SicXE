#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <Windows.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define f32 float
#define f64 double

#define b32 uint32_t

struct Symbol_Table_Entry
{
    char location[30];
    char label[30];
};

static void CreateStringOfSize(u32 count, char **result, b32 add_terminator)
{
    *result = (char *)malloc((count + 1) * sizeof(char));

    if(add_terminator)
    {
        result[count] = '\0'; // null terminator   
    }    
}

static void CreateStringArrayOfSize(u32 rows, u32 columns, char **result)
{
    *result = (char *)malloc((rows + columns) * sizeof(char));
}
    // Currently, heap allocations occur without deletion
    // TODO: Create memory arenas for different allocations
    // TODO: Dynamically allocate based on file size
    // TODO: Get data path dynamically 
int main(int argc, char **argv)
{
    char exe_path[260];
    GetModuleFileNameA(NULL, exe_path, sizeof(exe_path));

    char data_path[260];
    strcpy_s(data_path, sizeof(data_path), exe_path);
    strcat_s(data_path, "\\..\\data\\");
    strcat_s(data_path, "Example.txt");

    FILE *text_file = fopen("Example.txt", "rb");
    
    if(text_file == nullptr)
    {
        // log file not found
        return 0;
    }

    fseek (text_file , 0 , SEEK_END);
    u64 file_size = ftell (text_file);
    rewind (text_file);

    char *source_statement;
    CreateStringOfSize((u32)file_size, &source_statement, false);

    size_t diff = fread(source_statement, 1, (u32)file_size, text_file);

    char *modifiable_statement;
    CreateStringOfSize((u32)file_size, &modifiable_statement, false);

    strcpy(modifiable_statement, source_statement);

    char *format_one_instructions[6];
    format_one_instructions[0] = "FIX";
    format_one_instructions[1] = "FLOAT";
    format_one_instructions[2] = "HIO";
    format_one_instructions[3] = "NORM";
    format_one_instructions[4] = "SIO";
    format_one_instructions[5] = "TIO";

    char *format_two_instructions[11];
    format_two_instructions[0] = "ADDR";
    format_two_instructions[1] = "CLEAR";
    format_two_instructions[2] = "COMPR";
    format_two_instructions[3] = "DIVR";
    format_two_instructions[4] = "MULR";
    format_two_instructions[5] = "RMO";
    format_two_instructions[6] = "SHIFTL";
    format_two_instructions[7] = "SHIFTR";
    format_two_instructions[8] = "SUBR";
    format_two_instructions[9] = "SVC";
    format_two_instructions[10] = "TIXR";

   char label[30] = {};
   char instruction[30];
   char operand[30];
   char instruction_cache[200][30];
   char operand_cache[200][30];

    u32 index_into_instruction_cache = 0;
    u32 index_into_operand_cache = 0;

    // Arrays
    // fel satr, hn7otaha fe array
    // array, 2, 3

    char *words[2] = {"0"};
    u32 index_word = 0;
   char *newline_token = strtok(modifiable_statement, "\n");
   char *space_token;
    u32 program_start = 0;

    Symbol_Table_Entry entry_collection[200] = {};
    u32 entry_collection_index = 0;

   while(newline_token != NULL)
   {
        char line[400];
        strcpy(line, newline_token);

    u32 count = 1;
    for (u32 i = 0; newline_token[i] != '\0'; i++)
    {
        if (newline_token[i] == ' ' && newline_token[i+1] != ' ')
            count++;    
    }

    if(count == 3)
    {
        sscanf(line, "%s %s %s", label, instruction, operand);
        strcpy_s(entry_collection[entry_collection_index].label,
                sizeof(entry_collection[entry_collection_index].label),
                label);
        entry_collection_index++;
    }
    else
    {
        sscanf(line, "%s %s", instruction, operand);
    }

        
        newline_token = strtok(NULL, "\n");

    if(strcmp(instruction, "START") == 0)
    {
        program_start = atoi(operand);
    }

    else
    {

        strcpy_s(instruction_cache[index_into_instruction_cache],
                 sizeof(instruction_cache[index_into_instruction_cache]), 
                 instruction);

        index_into_instruction_cache++;

        strcpy_s(operand_cache[index_into_operand_cache],
                 sizeof(operand_cache[index_into_operand_cache]),
                 operand);

        index_into_operand_cache++;
        
    }
   }

    u32 location_counter[200] = {};
    location_counter[0] = program_start;
    u32 location_counter_index = 1;

    char format_four_token = '+';
    char format_five_token = '&';
    char format_six_token = '$';
    b32 is_format_one = false;
    b32 is_format_two = false;
    for(u32 instruction_index = 0; instruction_index < 6; ++instruction_index)
    {
        // directive checking
        // RESW. RESB. WORD. BYTE
        if(strcmp(instruction_cache[instruction_index], "RESB") == 0)
        {
            location_counter[location_counter_index] = location_counter[location_counter_index - 1] + atoi(operand_cache[instruction_index]);
            location_counter_index++;
        }
        else if(strcmp(instruction_cache[instruction_index], "RESW") == 0)
        {
            location_counter[location_counter_index] = location_counter[location_counter_index - 1] + (atoi(operand_cache[instruction_index]) * 3);
            location_counter_index++;
        }
        else if(strcmp(instruction_cache[instruction_index], "WORD") == 0)
        {
            location_counter[location_counter_index] = location_counter[location_counter_index - 1] + 3;
            location_counter_index++;
        }
        else if(strcmp(instruction_cache[instruction_index], "BYTE") == 0)
        {
            char comparable[30];
            strcpy_s(comparable, sizeof(comparable), operand_cache[instruction_index]);
            if(comparable[0] == 'X')
            {
                // hexadecimal
                // X'05'
                u32 count = (u32)strlen(operand_cache[instruction_index]);
                count -= 3;
                f64 size_of_increment = ceil((f64)(count / 2));

                location_counter[location_counter_index] = location_counter[location_counter_index - 1] + (u32)(size_of_increment);
                location_counter_index++;
            }
            else
            {
                // ASCII
                // C'EOF'
                u32 count = (u32)strlen(operand_cache[instruction_index]);
                count -= 3;

                location_counter[location_counter_index] = location_counter[location_counter_index - 1] + count;
                location_counter_index++;
            }
        }
        else
        {
            // format checking
            for(u32 format_one_index = 0; format_one_index < 6; ++format_one_index)
            {
                if(strcmp(instruction_cache[instruction_index], format_one_instructions[format_one_index]) == 0)
                {
                    is_format_one = true;
                    //instruction is format one (1 byte)
                    location_counter[location_counter_index] = location_counter[location_counter_index - 1] + 1;
                    location_counter_index++;
                }
            }

            for(u32 format_two_index = 0; format_two_index < 11; ++format_two_index)
            {
                if(strcmp(instruction_cache[instruction_index], format_two_instructions[format_two_index]) == 0)
                {
                    is_format_two = true;
                    //instruction is format 2 (2 bytes)
                    location_counter[location_counter_index] = location_counter[location_counter_index - 1] + 2;
                    location_counter_index++;
                }
            }

            if(!is_format_one && !is_format_two)
            {
                char comparable[30];
                strcpy_s(comparable,
                         sizeof(comparable), 
                         instruction_cache[instruction_index]);

                if(strcmp(instruction_cache[instruction_index], "BASE") == 0)
                {
                        location_counter[location_counter_index] = location_counter[location_counter_index - 1];
                        location_counter_index++;
                }
                else
                {
                    if(comparable[0] == format_four_token)
                    {
                        // format 4 (4 bytes)
                        location_counter[location_counter_index] = location_counter[location_counter_index - 1] + 4;
                        location_counter_index++;
                    }
                    else if(comparable[0] == format_five_token)
                    {
                        // format 5 (3 bytes)
                        location_counter[location_counter_index] = location_counter[location_counter_index - 1] + 3;
                        location_counter_index++;
                    }
                    else if(comparable[0] == format_six_token)
                    {
                        // format 6 (4 bytes)
                        location_counter[location_counter_index] = location_counter[location_counter_index - 1] + 4;
                        location_counter_index++;
                    }
                    else
                    {
                        // format 3 (3 bytes)
                        location_counter[location_counter_index] = location_counter[location_counter_index - 1] + 3;
                        location_counter_index++;
                    }
                }
            }
        }
    }

    char hexadecimal_location_counter[200][20] = {};
    // Time to get location counter values in hexadecimal
    char buffer[512];
    for(u32 location_index = 0; location_index < 200; ++location_index)
    {
		sprintf_s(buffer, sizeof(buffer), "%x\n", location_counter[location_index]);
        strcpy_s(hexadecimal_location_counter[location_index],
        sizeof(hexadecimal_location_counter[location_index]),
        buffer);

        strcpy_s(buffer,
        sizeof(buffer),
        " ");
    }

    for(u32 index = 0; index < 200; ++index)
    {
        strcpy_s(entry_collection[index].location, 
        sizeof(entry_collection[index].location),
        hexadecimal_location_counter[index]);
    }

    FILE *output_file = fopen("out.txt", "ab");

    if(output_file == NULL)
    {
        // Something is wrong
    }

    for (u32 location_index = 0; location_index < 200; ++location_index)
    {
         fprintf(output_file, "%s\n", hexadecimal_location_counter[location_index]);
    } 

    output_file = fopen("symbTable.txt", "ab");
    if (output_file == NULL)
    {
        // something is wrong
    }

    for (u32 symbol_index = 0; symbol_index < 200; ++symbol_index)
    {
        fprintf(output_file, "%s %s\n", entry_collection[symbol_index].location, entry_collection[symbol_index].label);
    }

    fclose(output_file);

    fclose(text_file);
    system("PAUSE");
    return(0);
}