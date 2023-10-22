% Interfata pentru identificare:
% 1. Comunicare seriala de la arduino in matlab (turatie, comanda, timp)
% 2. Salvarea datelor pentru identificare.
% 3. Afisarea pe un plot a datelor
% 4. Buton de Start/Stop communication
% 5. Resetarea datelor, se reimprospateaza vectorul care retine datele.


% Se importeaza datele (viteza masurata in rad/s si factorul de umplere)
% din fisierul Date_Identificare;

%viteza = DateIdentificare(:,1)';
%u = DateIdentificare(:,2)';
t = 0:1:130;

plot(t,u*6500,t,viteza);