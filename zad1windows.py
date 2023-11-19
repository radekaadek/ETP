import PySimpleGUI as sg
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

def ng_zero(wavelength): #wavelen in nm
    wavelength = wavelength / 1000 #wavelen in um
    return 287.6155 + 4.8866/(wavelength**2) + 0.0680/(wavelength**4)

def ew(wet_temp):
    return 6.1078 * np.exp((17.269 * wet_temp) / (237.30 + wet_temp))

def e(ew, pressure, dry_temp, wet_temp):
    return ew - 0.000662 * pressure * (dry_temp - wet_temp)

def ng(wavelength, dry_temp, pressure, e):
    return ng_zero(wavelength) * 0.269578 * pressure / (273.15 + dry_temp) - 11.27 * e / (273.15 + dry_temp)

def dD(wavelength, dry_temp, wet_temp, pressure):
    ewd = ew(wet_temp)
    ed = e(ewd, pressure, dry_temp, wet_temp)
    ngd = ng(wavelength, dry_temp, pressure, ed)
    ngn = ng(wavelength, 15, 1013.25, 10.87) # ng normalne
    return ngn - ngd

def arc_chord_difference(distance):
    earth_radius = 6378000
    dc = -distance**3 / (24 * earth_radius**2)
    return dc

def main():
    font = sg.DEFAULT_FONT
    start = 400
    stop = 1601
    step = 10
    x = list(range(start, stop, step))
    y = [ng_zero(i) for i in x]

    layout_tab1 = [
        [sg.Text('Zakladka 1: Obliczenie Ng dla dlugosci fali od 400 do 1600 nm', font=font)],
        [sg.Canvas(key='-CANVAS_TAB1-', pad=(0, 0), size=(650, 650))],
        [sg.Text('Tabela co 10 nm', font=font)],
        [sg.Table(values=[*zip(x, y)], headings=['Dlugosc fali (nm)', 'Wspolczynnik'], font=font, auto_size_columns=False, col_widths=[25, 25], justification='center', num_rows=10, key='-TABLE_TAB1-', enable_events=True)],
    ]

    layout_tab2 = [
        [sg.Text('Zakladka 2: Obliczenie poprawki atmosferycznej', font=font)],
        [sg.Text('Dlugosc fali (nm):', font=font), sg.InputText(key='-WAVELENGTH_TAB2-', font=font)],
        [sg.Text('Temperatura sucha (°C):', font=font), sg.InputText(key='-DRY_TEMP_TAB2-', font=font)],
        [sg.Text('Temperatura mokra (°C):', font=font), sg.InputText(key='-WET_TEMP_TAB2-', font=font)],
        [sg.Text('Cisnienie (hPa):', font=font), sg.InputText(key='-PRESSURE_TAB2-', font=font)],
        [sg.Text('Pomierzona dlugosc (m):', font=font), sg.InputText(key='-MEASURED_LENGTH_TAB2-', font=font)],
        [sg.Button('Oblicz korekte atmosferyczna', font=font)],
        [sg.Text('Poprawka na kilometr [mm]:', font=font), sg.Text('', key='-CORRECTION_KM_TAB2-', font=font)],
        [sg.Text('Poprawka do mierzonej dlugosci [m]:', font=font), sg.Text('', key='-CORRECTION_TO_MEASURED_LENGTH_TAB2-', font=font)],
        [sg.Text('Dlugosc poprawiona (m):', font=font), sg.Text('', key='-CORRECTED_LENGTH_TAB2-', font=font)],
    ]

    start = 1000
    stop = 100001
    step = 1000
    plt.clf()
    x = list(range(start, stop, step))
    y = [arc_chord_difference(i) for i in x]
    # meters to km
    x = [i / 1000 for i in x]

    layout_tab3 = [
        [sg.Text('Zakladka 3: Roznica miedzy lukem a cieciwa', font=font)],
        [sg.Canvas(key='-CANVAS_TAB3-', pad=(0, 0), size=(650, 400))],
        [sg.Text('Tabela co 1 km', font=font)],
        [sg.Table(values=[*zip(x, y)], headings=['Odleglosc (km)', 'Roznica (mm)'], font=font, auto_size_columns=False, col_widths=[25, 25], justification='center', num_rows=10, key='-TABLE_TAB3-', enable_events=True)],
    ]

    layout = [[sg.TabGroup([[sg.Tab('Zakladka 1', layout_tab1)],
                            [sg.Tab('Zakladka 2', layout_tab2)],
                            [sg.Tab('Zakladka 3', layout_tab3)]], font=font)],
              [sg.Button('Zamknij', font=font)]]
    
    window = sg.Window('Cwiczenie 1', layout, finalize=True)


    fig = plt.figure(1)
    canvas = FigureCanvasTkAgg(fig, master=window['-CANVAS_TAB1-'].TKCanvas)
    plot_widget = canvas.get_tk_widget()
    plot_widget.pack(side='top', fill='both', expand=1)


    start = 400
    stop = 1601
    step = 10
    plt.clf()
    x = list(range(start, stop, step))
    y = [ng_zero(i) for i in x]
    plt.plot(x, y, color='blue')
    plt.xlabel('Dlugosc fali (nm)')
    plt.ylabel('Wspolczynnik')
    plt.title('Zaleznosc wspolczynnikow od dlugosci fali')
    plt.grid()
    # draw on canvas
    fig.canvas.draw()

    fig = plt.figure(2)
    canvas = FigureCanvasTkAgg(fig, master=window['-CANVAS_TAB3-'].TKCanvas)
    plot_widget = canvas.get_tk_widget()
    plot_widget.pack(side='top', fill='both', expand=1)

    start = 1000
    stop = 100001
    step = 1000
    plt.clf()
    x = list(range(start, stop, step))
    y = [arc_chord_difference(i) for i in x]
    # meters to km
    x = [i / 1000 for i in x]
    plt.plot(x, y, color='blue')
    plt.xlabel('Odleglosc (km)')
    plt.ylabel('Roznica (mm)')
    plt.title('Roznica miedzy lukiem a cieciwa')
    plt.grid()
    # draw on canvas
    fig.canvas.draw()


    while True:
        event, values = window.read()

        if event in (sg.WINDOW_CLOSED, 'Zamknij'):
            break

        elif event == 'Oblicz korekte atmosferyczna':
            # get values from input
            try:
                wavelength = float(values['-WAVELENGTH_TAB2-'])
                dry_temp = float(values['-DRY_TEMP_TAB2-'])
                wet_temp = float(values['-WET_TEMP_TAB2-'])
                pressure = float(values['-PRESSURE_TAB2-'])
                measured_length = float(values['-MEASURED_LENGTH_TAB2-'])
            except:
                sg.popup('Wprowadzono niepoprawne dane!')
                continue
            if wavelength <= 0 or dry_temp <= -273.15 or wet_temp <= -273.15 or pressure <= 0 or measured_length <= 0:
                sg.popup('Wprowadzono niepoprawne dane!')
                continue
            # calculate correction
            dc = dD(wavelength, dry_temp, wet_temp, pressure)
            correction_km = dc
            # correction km is mm/km
            correction_to_measured_length = dc * measured_length / 1000000
            # correction to measured length is mm
            corrected_length = measured_length + correction_to_measured_length
            

            # update output
            window['-CORRECTION_KM_TAB2-'].update(f"{correction_km:.2f}")
            window['-CORRECTION_TO_MEASURED_LENGTH_TAB2-'].update(f"{correction_to_measured_length:.4f}")
            window['-CORRECTED_LENGTH_TAB2-'].update(f"{corrected_length:.4f}")


    window.close()

if __name__ == '__main__':
    main()
