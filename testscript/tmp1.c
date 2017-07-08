int f(int a){
	if(a <= 2){
		return 1;
	}
	else{
		return f(a-1)+f(a-2);
	}
}

int main(){
	return f(8);
}