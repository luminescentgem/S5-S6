import type DTO         from "../../Interface/DTO.js"
import type Query       from "../../Interface/Query.js"

export default abstract class AbstractRepository {
    abstract insert(dto: DTO): Promise<string>
    abstract update(dto: DTO): Promise<string>
    abstract find(query: Query): Promise<DTO>
    abstract findAll(query: Query): Promise<DTO[]>
    abstract delete(query: Query): Promise<DTO[]>
}