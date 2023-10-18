% Interfata pentru identificare:
% 1. Comunicare seriala de la arduino in matlab (turatie, comanda, timp)
% 2. Salvarea datelor pentru identificare.
% 3. Afisarea pe un plot a datelor
% 4. Buton de Start/Stop communication
% 5. Resetarea datelor, se reimprospateaza vectorul care retine datele.

% Comunicare Seriala:

arduinoObj = serialport("COM3",9600);
configureTerminator(arduinoObj,"CR/LF");
arduinoObj.UserData = struct("Comanda",[],"Timp",1);

configureCallback(arduinoObj,"terminator",@readData);


function readData(src,~)

data = readline(src);

src.UserData.Comanda(end + 1) = str2double(data);
src.UserData.Timp = src.UserData.Timp + 1;

if src.UserData.Timp > 1001
    configureCallback(src,"off");
    plot(src.UserData.Comanda(2:end));
end
end

