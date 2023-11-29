// Prevents additional console window on Windows in release, DO NOT REMOVE!!
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

fn main() {
  let ports = serialport::available_ports().expect("No ports found!");
  for p in ports {
      println!("{}", p.port_name);
  }
  tauri::Builder::default()
    .run(tauri::generate_context!())
    .expect("error while running tauri application");
}
