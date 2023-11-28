import serial
import PySimpleGUI as sg

def open_port(port: str, baudrate: int, timeout: int, stopbits: float, bytesize: int, parity: str, xonxoff: str, rtscts: str) -> serial.Serial:
    """Opens serial port with given parameters. If it fails, returns None.

    Keyword arguments:
    port -- port name
    baudrate -- baudrate
    timeout -- timeout (Limited, Infinite)
    stopbits -- stopbits
    bytesize -- bytesize
    parity -- parity (None, Odd, Even, Mark, Space)
    xonxoff -- xonxoff (Off, On)
    rtscts -- rtscts (Off, On)

    Returns:
    serial.Serial -- serial port
    """
    if parity == "None":
        parity = "N"
    if timeout == "Limited":
        timeout = "1"
    elif timeout == "Infinite":
        timeout = "0"
    if xonxoff == "Off":
        xonxoff = "0"
    elif xonxoff == "On":
        xonxoff = "1"
    if rtscts == "Off":
        rtscts = "0"
    elif rtscts == "On":
        rtscts = "1"
    try:
        ser = serial.Serial(port,
                            baudrate=baudrate,
                            timeout=timeout,
                            stopbits=stopbits,
                            bytesize=bytesize,
                            parity=parity,
                            xonxoff=xonxoff,
                            rtscts=rtscts)
    except serial.SerialException:
        return None
    return ser


# layout
layout = [
    [sg.Text('Port COM', size=(10, 1)), sg.Combo(['COM4', 'COM1', 'COM2', 'COM3', 'COM6'], default_value='COM4', key='-PORT-')],
    [sg.Text('Baud rate', size=(10, 1)), sg.Combo(['1200', '2400', '4800', '9600', '14400', '19200', '38400', '56000'], default_value='9600', key='-BAUDRATE-')],
    [sg.Text('Czas oczekiwania', size=(10, 1)), sg.Combo(['1', '0'], default_value='1', key='-TIMEOUT-')],
    [sg.Text('Bity stopu', size=(10, 1)), sg.Combo(['1', '1.5', '2'], default_value='1', key='-STOPBITS-')],
    [sg.Text('Bity danych', size=(10, 1)), sg.Combo(['5', '6', '7', '8'], default_value='8', key='-DATABITS-')],
    [sg.Text('Parzystosc', size=(10, 1)), sg.Combo(["None", "Odd", "Even", "Mark", "Space"], default_value="None", key='-PARITY-')],
    [sg.Text('Xon/Xoff', size=(10, 1)), sg.Combo(["Off", 'On'], default_value="Off", key='-XONXOFF-')],
    [sg.Text('Rts/Cts', size=(10, 1)), sg.Combo(["Off", 'On'], default_value="Off", key='-RTSCTS-')],
    [sg.Button('Otworz port', key='-OPEN-'), sg.Button('Zamknij port', key='-CLOSE-'), sg.Button('Pobierz dane', key='-READ-'), sg.Button('Zapisz dane', key='-SAVE-')],
    [sg.Multiline(size=(80, 20), key='-OUTPUT-')]
]
# window
window = sg.Window('Zadanie 1', layout, finalize=True)
ser: serial.Serial = None
# event loop
while True:
    event, values = window.read()
    if event == sg.WIN_CLOSED:
        break
    elif event == '-OPEN-':
        ser = open_port(values['-PORT-'],
                                int(values['-BAUDRATE-']),
                                int(values['-TIMEOUT-']),
                                float(values['-STOPBITS-']),
                                int(values['-DATABITS-']),
                                values['-PARITY-'],
                                values['-XONXOFF-'],
                                values['-RTSCTS-'])
        if ser is None:
            sg.popup('Nie mozna otworzyc portu!')
            continue
        if ser.isOpen():
            print(ser.name + ' is open')
        else:
            print(ser.name + ' is closed')
            sg.popup('Port jest zamkniety!')
    elif event == '-CLOSE-':
        ser = open_port(values['-PORT-'],
                        int(values['-BAUDRATE-']),
                        int(values['-TIMEOUT-']),
                        float(values['-STOPBITS-']),
                        int(values['-DATABITS-']),
                        values['-PARITY-'],
                        values['-XONXOFF-'],
                        values['-RTSCTS-'])
        if ser is None:
            sg.popup('Nie mozna otworzyc portu do zamkniecia!')
            continue
        if ser.isOpen():
            ser.close()
            print(ser.name + ' is closed')
        else:
            sg.popup('Port jest juz zamkniety!')
    elif event == '-READ-':
        if ser is None:
            sg.popup('Nie otwarto portu!')
            continue
        try:
            line = ser.readline()
        except serial.SerialException:
            sg.popup('Nie mozna odczytac danych!')
            continue
        window['-OUTPUT-'].print(line)
    elif event == '-SAVE-':
        try:
            name = sg.popup_get_file('Zapisz plik', save_as=True, no_window=True)
            save = open(name, 'w')
            save.writelines(window['-OUTPUT-'].get())
            save.close()
        except FileNotFoundError:
            sg.popup('Nie znaleziono pliku!')
            continue
        except:
            sg.popup('Nie mozna zapisac pliku!')
            continue
window.close()

