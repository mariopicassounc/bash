## Informe 
- Muy bueno, pueden seguir con este formato de informe para el resto de la materia. Incluso tienen margen para hacerlo menos exhaustivo

## Repo 
- Autoría despareja de commits

## Código
- En general muy bien. Claro y prolijo
- `define`s para separadores
- Quizás tratar redirecciones in y out en una misma función?
- En el informe mencionan que tuvieron que cubrir muchos casos de errores. Se puede considerar agrupar errores con el tradeoff de que los mensajes de error serán menos informativos.

## Funcionalidad
- Testeo 100%
- Parser no detecta algunos comandos mal formados `ls |`, `ls >`
- `builtin`s dentro de pipes, bien que imitaron a bash de linux 
- Pueden liberar la memoria de `argv` en caso de falla en `execvp`
- El padre puede cerrar todas las puntas del pipe
