from ast import While
from numpy import NAN, NaN
import pandas as pd
import numpy as np
from datetime import datetime



cols = ['Код','Название', 'Адрес', 'Запись', 'Мин.', 'Макс.', 'Завод. установ.', 'Описание', 'Eeprom']

def isGroup(var, list):
    for str in list:
        if str.startswith(var + '.'):
            return True
    return False

print("Введите название файла с расширением")
file_name = input()
mviewer_project = pd.read_excel(file_name, usecols=cols)


cods = mviewer_project['Код'].tolist()
names = mviewer_project['Название'].tolist()
adr = mviewer_project['Адрес'].tolist()
mode_rw = mviewer_project['Запись'].tolist()
min_val = mviewer_project['Мин.'].tolist()
max_val = mviewer_project['Макс.'].tolist()
default_val = mviewer_project['Завод. установ.'].tolist()
descr = mviewer_project['Описание'].tolist()
eeprom = mviewer_project['Eeprom'].tolist()


for elem in names:
    if(elem.find(' ') != -1):
        raise ValueError('Имена переменных и групп не должны содержать пробелов ')

# для начала определим количество глобальных переменных (считаем, что переменные ОБЯЗАТЕЛЬНО 32-битные)
num_var = 0
for elem in adr:
    if(not np.isnan(elem)):
        num_var += 1

print("Количество глобальных переменных = "+str(num_var))


var_names_in_unuon = []
first_obj = cods[0]


groups = []
vars = []

for elem in cods:
    if(isGroup(elem,cods)):
        groups.append(elem)
    else:
        vars.append(elem)



for elem in vars:
    var_names_in_unuon.append('.' + names[cods.index(elem)])

vars_empty = False
# цикл пока не опустошится список vars
while(not vars_empty):
    # удаляем все до последей точки
    for i in range(len(vars)):
        # print(elem)
        vars[i] = vars[i][0:vars[i].rfind('.')]
    # записываем название групп переменных
    temp_list = []
    for counter in range(len(vars)):
        # print(counter,"\t\t", vars[counter], "\t\t",var_names_in_unuon[counter] )
        elem = vars[counter]
        if elem != '':
            temp_list.append('.' + names[cods.index(elem)]) #получили название групп
        else:
            temp_list.append('')
    # print()
    # print()
    # записываем полученное на предыдущем этапе в начало
    for i in range(len(vars)):
        var_names_in_unuon[i] = temp_list[i] + var_names_in_unuon[i]

    vars_empty = True

    for i in range(len(vars)):
        if(vars[i] != ''):
            vars_empty = False

# убираем точку из начала (можно сразу добавить амперсанд)  
for i in range(len(var_names_in_unuon)):
    var_names_in_unuon[i] = var_names_in_unuon[i][1:len(var_names_in_unuon[i])]





adr_list = []
for elem in cods:
    if(not isGroup(elem,cods)):
        adr_list.append(adr[cods.index(elem)])



temp_list1 = []
for elem in sorted(adr_list):
    temp = var_names_in_unuon[adr_list.index(elem)]
    temp_list1.append(temp)

var_names_in_unuon = temp_list1


var_names_without_gl_var_with_mode = []
mb_mode_list = []

for elem in cods:
    if(not isGroup(elem,cods)):
       mb_mode_list.append(mode_rw[cods.index(elem)]) 

for elem in var_names_in_unuon:
    point_index = elem.find('.')
    if(mb_mode_list[var_names_in_unuon.index(elem)] == 1):
        var_names_without_gl_var_with_mode.append("c_" + elem[(point_index + 1):len(elem)])
    else:
        var_names_without_gl_var_with_mode.append("a_" + elem[(point_index + 1):len(elem)])






header_file = open('config_global_var_values.h', 'w')

cur_datetime = datetime.now()
header_file.write("/*\n * config_global_var_values.h\n *\n * Created on: " + str(cur_datetime.day)+'.'+str(cur_datetime.month)+'.'+str(cur_datetime.year) + "\n * \t Author: Denis Pavlov\n */\n")
header_file.write("#ifndef GLOBAL_VAR_CONFIG_GLOBAL_VAR_VALUES_H_\n#define GLOBAL_VAR_CONFIG_GLOBAL_VAR_VALUES_H_\n")
header_file.write("#include <stdbool.h>\n#include <stdint.h>\n#include \"main.h\"\n#include \"global_var.h\"\n\n#define NUM_OF_GLOBAL_VAR \t " + str(num_var) + "\n\n")

header_file.write("typedef union{\n\tstruct {\n")
for elem in var_names_without_gl_var_with_mode:
    header_file.write("\t\tuint32_t " + elem.replace('.','_') + ';\n')
header_file.write("\t} var;\n")
header_file.write("\tuint32_t arr[NUM_OF_GLOBAL_VAR];\n} globalVarValues;\n\n")

header_file.write("typedef union{\n\tstruct {\n")
# цикл по всем названиям
for elem in var_names_without_gl_var_with_mode:
    header_file.write("\t\tmodbusAccess " + elem.replace('.','_') + ';\n')
header_file.write("\t} var;\n")
header_file.write("\tmodbusAccess arr[NUM_OF_GLOBAL_VAR];\n} globalVarModbusAccess;\n\n")

header_file.write("//в данном перечислении записываются все переменные, для которых разрешена ЗАПИСЬ, последний элемент определяет общее количество.\n")
header_file.write("typedef enum {\n")
var_num = 0
# получим названия всех переменных, записываемых в eeprom



eeprom_saving = []
for elem in sorted(adr_list):
    if(not isGroup(cods[adr.index(elem)],cods)):
        eeprom_saving.append(eeprom[adr.index(elem)])


# eeprom_saving = []
# for elem in cods:
#     if(not isGroup(elem,cods)):
#         eeprom_saving.append(eeprom[cods.index(elem)])

eeprom_enum_elem_names = []

for elem in var_names_without_gl_var_with_mode:
    temp = elem.replace('.',' ')
    temp = temp.replace('_', ' ')
    temp = temp.title()
    temp = 'k' + temp.replace(' ', '')[1:len(temp.replace(' ', ''))]
    if (eeprom_saving[var_names_without_gl_var_with_mode.index(elem)] == 1):
        eeprom_enum_elem_names.append(temp)
        header_file.write('\t' + temp + ' = ' + str(var_num) + ',\n')
        var_num += 1
header_file.write('\n\tkVarNum = ' + str(var_num) + '\n')
header_file.write("}VarName;\n\n")



header_file.write("extern const globalVarValues global_var_default_value;\n")
header_file.write("extern const globalVarValues global_var_max_value;\n")
header_file.write("extern const globalVarValues global_var_min_value;\n")
header_file.write("extern const globalVarModbusAccess global_var_modbus_access;\n")
header_file.write("extern uint32_t* cur_var_value[kVarNum];\n")
header_file.write("extern var* var_pointers[NUM_OF_GLOBAL_VAR];\n")
header_file.write("extern uint16_t varIdList[kVarNum];\n")
header_file.write("extern globalVar global_var;\n\n")
header_file.write("uint32_t GetDefaultValue(uint16_t varId);\n\n")
header_file.write("#endif /* GLOBAL_VAR_CONFIG_GLOBAL_VAR_VALUES_H_ */")




header_file.close()

source_file = open('config_global_var_values.c', 'w')


source_file.write("/*\n * config_global_var_values.h\n *\n * Created on: " + str(cur_datetime.day)+'.'+str(cur_datetime.month)+'.'+str(cur_datetime.year) + "\n * \t Author: Denis Pavlov\n */\n")
source_file.write("#include \"config_global_var_values.h\"\n\n")

source_file.write("uint16_t varIdList[kVarNum];\n\n//данный экземпляр структуры определяет значения по умолчанию всех глобальных переменных.\n")
source_file.write("const globalVarValues global_var_default_value = {\n")



default_values = []
for elem in sorted(adr_list):
    if(not isGroup(cods[adr.index(elem)],cods)):
        if(not np.isnan(default_val[adr.index(elem)])):
            default_values.append(int(default_val[adr.index(elem)]))
        else:
            default_values.append(int(0))



# default_values = []
# for elem in cods:
#     if(not isGroup(elem,cods)):
#         if(not np.isnan(default_val[cods.index(elem)])):
#             default_values.append(int(default_val[cods.index(elem)]))
#         else:
#             default_values.append(int(0))


for elem in var_names_without_gl_var_with_mode:
    temp = '.var.' + elem.replace('.','_')
    source_file.write('\t' + temp + ' = ' + str(default_values[var_names_without_gl_var_with_mode.index(elem)]) + ',\n')
source_file.write("};\n\n")




source_file.write("//данный экземпляр структуры определяет максимальное значение всех глобальных переменных.\n")
source_file.write("const globalVarValues global_var_max_value = {\n")



max_values = []
for elem in sorted(adr_list):
    if(not isGroup(cods[adr.index(elem)],cods)):
        if(not np.isnan(max_val[adr.index(elem)])):
            max_values.append(int(max_val[adr.index(elem)]))
        else:
            max_values.append(int(0))


# max_values = []
# for elem in cods:
#     if(not isGroup(elem,cods)):
#         if(not np.isnan(max_val[cods.index(elem)])):
#             max_values.append(int(max_val[cods.index(elem)]))
#         else:
#             max_values.append(int(0))


for elem in var_names_without_gl_var_with_mode:
    temp = '.var.' + elem.replace('.','_')
    source_file.write('\t' + temp + ' = ' + str(max_values[var_names_without_gl_var_with_mode.index(elem)]) + ',\n')
source_file.write("};\n\n")




source_file.write("//данный экземпляр структуры определяет минимальное значение всех глобальных переменных.\n")
source_file.write("const globalVarValues global_var_min_value = {\n")

min_values = []
for elem in sorted(adr_list):
    if(not isGroup(cods[adr.index(elem)],cods)):
        if(not np.isnan(min_val[adr.index(elem)])):
            min_values.append(int(min_val[adr.index(elem)]))
        else:
            min_values.append(int(0))


# min_values = []
# for elem in cods:
#     if(not isGroup(elem,cods)):
#         if(not np.isnan(min_val[cods.index(elem)])):
#             min_values.append(int(min_val[cods.index(elem)]))
#         else:
#             min_values.append(int(0))


for elem in var_names_without_gl_var_with_mode:
    temp = '.var.' + elem.replace('.','_')
    source_file.write('\t' + temp + ' = ' + str(min_values[var_names_without_gl_var_with_mode.index(elem)]) + ',\n')
source_file.write("};\n\n")



source_file.write("//данный экземпляр структуры определяет доступ к всем глобальным переменным.\n")
source_file.write("const globalVarModbusAccess global_var_modbus_access = {\n")



# mode_read_write = []
# for elem in sorted(adr_list):
#     if(not isGroup(cods[adr.index(elem)],cods)):
#         if(not np.isnan(min_val[adr.index(elem)])):
#             min_values.append(int(min_val[adr.index(elem)]))
#         else:
#             min_values.append(int(0))



mode_read_write = []
for elem in sorted(adr_list):
    if(not isGroup(cods[adr.index(elem)],cods)):
        if(not np.isnan(mode_rw[adr.index(elem)])):
            if(mode_rw[adr.index(elem)] == 1):
                mode_read_write.append("kModbusWrite")
            else:
                mode_read_write.append("kModbusRead")
        else:
            mode_read_write.append("kModbusRead")


for elem in var_names_without_gl_var_with_mode:
    temp = '.var.' + elem.replace('.','_')
    source_file.write('\t' + temp + ' = ' + str(mode_read_write[var_names_without_gl_var_with_mode.index(elem)]) + ',\n')
source_file.write("};\n\n")


source_file.write("//данный массив сформирован из ссылок на значения глобальных переменных, которые записывются в ПЗУ (в соответствие с перечислением)\n")
source_file.write("uint32_t* cur_var_value[kVarNum] = {\n")

# возможно цикл ниже не обязателен
# eeprom_saving = []
# for elem in cods:
#     if(not isGroup(elem,cods)):
#         eeprom_saving.append(eeprom[cods.index(elem)])

# default_values = []
# for elem in sorted(adr_list):
#     if(not isGroup(cods[adr.index(elem)],cods)):
#         if(not np.isnan(default_val[adr.index(elem)])):
#             default_values.append(int(default_val[adr.index(elem)]))
#         else:
#             default_values.append(int(0))

eeprom_saving_names = []
for elem in var_names_without_gl_var_with_mode:
    if (eeprom_saving[var_names_without_gl_var_with_mode.index(elem)] == 1):
        eeprom_saving_names.append(elem)


for elem in var_names_in_unuon:
    if (eeprom_saving[var_names_in_unuon.index(elem)] == 1):
        source_file.write('\t&' + elem + '.value.all,\n')
source_file.write("};\n\n")


source_file.write("//данный массив содержит ссылки на ВСЕ глобальные переменные.\n//В соответствие с этим массивом раздаются адреса пееменным по порядку с увеличением адреса с увеличением элемента массива\n")
source_file.write("var* var_pointers[NUM_OF_GLOBAL_VAR] = {\n")


for elem in adr_list:
    temp = var_names_in_unuon[adr_list.index(elem)]
    source_file.write('\t&' + temp + ',\n')

source_file.write("};\n\n")




source_file.write("//данная функция возвращает значение по умолчанию для переменных, записываемых в ПЗУ\n")
source_file.write("uint32_t GetDefaultValue(uint16_t varId)\n{\n\n\tuint32_t res = 0;\n\n")


first_flag = True
for elem in eeprom_saving_names:
    temp = '.var.' + elem.replace('.','_')
    if(first_flag):
        first_flag = False
        source_file.write("\tif (varId == varIdList[" + eeprom_enum_elem_names[eeprom_saving_names.index(elem)] + "])\t{\n")
        source_file.write("\t\tres = global_var_default_value" + temp + ";\n\t}\n")
    else:
        source_file.write("\telse if (varId == varIdList[" + eeprom_enum_elem_names[eeprom_saving_names.index(elem)] + "])\t{\n")
        source_file.write("\t\tres = global_var_default_value" + temp + ";\n\t}\n")
source_file.write("\treturn res;\n}\n")





source_file.close()

