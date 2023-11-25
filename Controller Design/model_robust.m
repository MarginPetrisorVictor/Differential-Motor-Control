T = 0.8;
Knom = 2225;

u = 0.05:0.01:0.2;
k = [2060 1883.33 1742.86 1637.5 1533.33 1460 1390.91 1333.33 1284.62 1235.71 1186.67 1156.25 1123.53 1094.44 1057.89 1010];
v = [103 113 122 131 138 146 153 160 167 174 178 185 191 197 201 205];

figure
plot(u,k);
title('K = f(u)')
xlabel('Comanda[%]')
ylabel('Gain')
figure
plot(v,k);
title('K = f(viteza)')
xlabel('Viteza[RPS]')
ylabel('Gain')