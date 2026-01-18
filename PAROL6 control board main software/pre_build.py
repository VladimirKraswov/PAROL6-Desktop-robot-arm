Import("env")
import os

def fix_variant_conflict(env):
    print("=== Fixing STM32 generic variant conflict ===")
    
    fw_dir = env.PioPlatform().get_package_dir("framework-arduinoststm32")
    generic_cpp = os.path.join(fw_dir, "variants", "generic", "variant_generic.cpp")
    
    if os.path.exists(generic_cpp):
        print(f"Found generic variant: {generic_cpp}")
        
        # Делаем резервную копию
        backup = generic_cpp + ".bak"
        if not os.path.exists(backup):
            os.rename(generic_cpp, backup)
            print(f"Renamed to {backup}")
        
        # Создаём пустой файл вместо него
        with open(generic_cpp, 'w') as f:
            f.write("// Intentionally empty to avoid multiple definitions\n")
        print("Created empty variant_generic.cpp")
    else:
        print("Generic variant not found — maybe already fixed?")

# Выполняем fix при инициализации
fix_variant_conflict(env)