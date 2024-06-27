%A = [-48580.9682804674 2222036.72787980;-0.0390000000000000 -0.500000000000000];
%B = [0; 10];
%C = [1 0];

setlmis([])

P = lmivar(1,[2 1]);
Z = lmivar(2,[2 2]);
alfa = 10;

lmiterm([1 1 1 P], A,1,'s');
lmiterm([1 1 1 P], 2*alfa, 1);
lmiterm([1 1 1 Z], B, 1, 's');

theta = 0.01;
lmiterm([2 1 1 P], A, 1, 's');
lmiterm([2 1 1 Z], B, 1, 's');
lmiterm([2 1 2 P], A, 1/tan(theta));
lmiterm([2 1 2 P], 1/tan(theta), -A');
lmiterm([2 1 2 Z], B, 1/tan(theta));
lmiterm([2 1 2 -Z], 1/tan(theta), -B');
lmiterm([2 2 2 P], A, 1, 's');
lmiterm([2 2 2 Z], B, 1, 's');

umax = 1;
Q = 1/2*[(1/220)^2 0; 0 (1/20)^2];

lmiterm([3 1 1 0], -eye(2)*umax^2);
lmiterm([3 2 1 -Z], -1, 1);
lmiterm([3 2 2 0], -Q);

lmiterm([-4 1 1 P], 1, 1);

lmis = getlmis;

[tmin, xfes]= feasp(lmis);

P = dec2mat(lmis,xfes, P);
Z = dec2mat(lmis,xfes, Z);

K = Z/P;



