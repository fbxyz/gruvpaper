#!/usr/bin/env bash

killall -q gruvpaper.sh

# Create the wallpaper directory if it doesn't exist
output_dir="/tmp/wallpaper"

#gruvpaper location
gruvpaper="$HOME/.config/gruvpaper/gruvpaper"

# Create output directory if it doesn't exist
mkdir -p "$output_dir"

calculate_log() {
    local value=$1
    echo "l($value+1)" | bc -l | awk '{print int($1)}'
}

calculate_exp() {
    local value=$1
    echo "e($value)" | bc -l | awk '{print int($1)}'
}

calculate_sqrt() {
    local value=$1
    echo "sqrt($value)" | bc -l | awk '{print int($1)}'
}

i=1
while [ true ]
	do

  # Collect system metrics
  cpu_usage=$(grep 'cpu ' /proc/stat | awk '{usage=($2+$4)*100/($2+$4+$5)} END {print usage}')
  mem_usage=$(free | grep Mem | awk '{print $3/$2 * 100.0}')
  load_average=$(cat /proc/loadavg | awk '{print $1 * 100.0}')
  cpu_temp=$(sensors | grep 'Tctl' | awk '{print $2}' | tr -d '+°C')
  task_count=$(ps aux | wc -l)

  # Apply geometric transformations (example: log transformation)
  cpu_usage_exp=$(calculate_exp $cpu_usage)
  load_average_exp=$load_average
  cpu_temp_log=$cpu_temp
  task_count_sqrt=$(calculate_sqrt $task_count)

  # Check if the values are valid
  if [[ -z $cpu_usage_exp || -z $mem_usage || -z $load_average_exp || -z $cpu_temp_log || -z $task_count_sqrt ]]; then
      echo "Error: One or more calculated log values are empty or invalid."
      exit 1
  fi

  #echo "CPU Usage : $cpu_usage_exp"
  #echo "Memory Usage : $mem_usage"
  #echo "Load Average : $load_average_exp"
  #echo "CPU Temp Log: $cpu_temp_log"
  #echo "Task Count sqrt: $task_count_sqrt"

  output_filename=wall_${i}.png

  # Generate image with gruvpaper
  "$gruvpaper" \
    -d "$output_dir" \
    -f "$output_filename" \
    -w 1920 \
    -h 1080 \
    -s 14 \
    -i 10 \
    -o 50 \
    -b "#32302F" \
    -e "#504945" \
    -r "#665C54" \
    -c "#458588,#689D6A,#98971A,#D79921,#D65D0E" \
    $cpu_usage_exp $mem_usage $load_average_exp $cpu_temp_log $task_count_sqrt


    # Set wallpaper to the newly generated image
		feh --bg-fill $output_dir/$output_filename --bg-fill $output_dir/$output_filename

    i=$(( ${i}*-1 ))
    sleep 2
  done
