#!/bin/bash

# SEQUENCER EEPROM CONFIGURATION SCRIPT
echo "=========================================="
echo "    SEQUENCER EEPROM CONFIGURATOR"
echo "=========================================="
echo ""

# Function to show available environments
show_environments() {
    echo "Available environments:"
    echo "1. eeprom_config_AVRISPmkII - For AVRISP mkII programmer"
    echo "2. eeprom_config_USBasp     - For USBasp programmer"
    echo ""
}

# Function to flash EEPROM config
flash_eeprom() {
    local env=$1
    echo "Flashing EEPROM configuration using environment: $env"
    echo "This will:"
    echo "  - Set default BPM, clock settings, etc."
    echo "  - Configure default Euclidean patterns"
    echo "  - Create basic step sequencer patterns"
    echo ""
    
    echo "Building and uploading EEPROM configuration..."
    platformio run -e "$env" --target upload
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "✓ EEPROM configuration complete!"
        echo ""
        echo "IMPORTANT: After flashing EEPROM config, you should:"
        echo "1. Open Serial Monitor (115200 baud) to verify settings"
        echo "2. Flash your main sequencer program"
        echo "3. Your sequencer will boot with the new default settings"
    else
        echo ""
        echo "✗ Upload failed! Check your connections and try again."
    fi
}

# Function to flash main program
flash_main() {
    local env=$1
    echo "Flashing main sequencer program using environment: $env"
    
    echo "Building and uploading main program..."
    platformio run -e "$env" --target upload
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "✓ Main program upload complete!"
        echo "Your module is ready to use!"
        echo ""
        echo "Exiting script..."
        exit 0
    else
        echo ""
        echo "✗ Upload failed! Check your connections and try again."
    fi
}

# Main menu
while true; do
    echo "What would you like to do?"
    echo ""
    echo "1. Flash both (EEPROM config first, then main program)"
    echo "2. Flash main sequencer program only"
    echo "3. Exit"
    echo ""
    
    read -p "Choose an option (1-3): " choice
    echo ""
    
    case $choice in
        1)
            show_environments
            read -p "Choose environment (1-2): " env_choice
            case $env_choice in
                1) 
                    flash_eeprom "eeprom_config_AVRISPmkII"
                    echo ""
                    read -p "Press Enter to continue with main program..." 
                    flash_main "program_via_AVRISPmkII"
                    ;;
                2) 
                    flash_eeprom "eeprom_config_USBasp"
                    echo ""
                    read -p "Press Enter to continue with main program..."
                    flash_main "program_via_USBasp"
                    ;;
                *) echo "Invalid choice!" ;;
            esac
            ;;
        2)
            show_environments
            read -p "Choose environment (1-2): " env_choice
            case $env_choice in
                1) flash_main "program_via_AVRISPmkII" ;;
                2) flash_main "program_via_USBasp" ;;
                *) echo "Invalid choice!" ;;
            esac
            ;;
        3)
            echo "Goodbye!"
            exit 0
            ;;
        *)
            echo "Invalid choice! Please choose 1-3."
            ;;
    esac
    
    echo ""
    echo "----------------------------------------"
    echo ""
done
