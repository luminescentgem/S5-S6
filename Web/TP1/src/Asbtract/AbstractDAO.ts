import Database         from '@/database/connection.js'
import type Query       from '@/Interface/Query.js'
import type Entity      from '@/Interface/Entity.js'
import type DTO         from '@/Interface/DTO.js'

const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789'

export default abstract class AbstractDAO {
    protected db

    constructor() {
        this.db = Database.getDb()
        if (!this.db) {
            throw new Error('Database instance is not available')
        }
    }

    generateRandomID(maxLength = 10) {
        const length = Math.floor(Math.random() * 10**(maxLength-1) + 1).toString().length // Random length from 1 to maxLength
        let id = ''
        for (let i = 0; i < length; i++) {
            id += chars[Math.floor(Math.random() * chars.length)]
        }
        return id
    }
    
    // return true if an element matches, false otherwise
    abstract exists(query: Query): Promise<boolean>

    // find the first element that matches
    abstract find(query: Query): Promise<Entity>
    
    abstract findAll(query: Query): Promise<Entity[]>

    abstract insert(entity: Query): Promise<string>

    abstract update(dto: DTO): Promise<string>

    abstract delete(query: Query): Promise<Entity[]>
}
 